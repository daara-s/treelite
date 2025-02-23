"""Tests for scikit-learn integration"""

import numpy as np
import pytest
from packaging.version import parse as parse_version

import treelite

try:
    from hypothesis import assume, given, settings
    from hypothesis.strategies import data as hypothesis_callback
    from hypothesis.strategies import floats, integers, just, sampled_from
except ImportError:
    pytest.skip("hypothesis not installed; skipping", allow_module_level=True)

try:
    from sklearn import __version__ as sklearn_version
    from sklearn.datasets import make_classification
    from sklearn.dummy import DummyClassifier, DummyRegressor
    from sklearn.ensemble import (
        ExtraTreesClassifier,
        ExtraTreesRegressor,
        GradientBoostingClassifier,
        GradientBoostingRegressor,
        HistGradientBoostingClassifier,
        HistGradientBoostingRegressor,
        IsolationForest,
        RandomForestClassifier,
        RandomForestRegressor,
    )
    from sklearn.utils import shuffle
except ImportError:
    pytest.skip("scikit-learn not installed; skipping", allow_module_level=True)

try:
    import pandas as pd
except ImportError:
    pytest.skip("Pandas required", allow_module_level=True)


from .hypothesis_util import (
    standard_classification_datasets,
    standard_regression_datasets,
    standard_settings,
)
from .util import to_categorical


@given(
    clazz=sampled_from(
        [
            RandomForestRegressor,
            ExtraTreesRegressor,
            GradientBoostingRegressor,
            HistGradientBoostingRegressor,
        ]
    ),
    n_estimators=integers(min_value=5, max_value=10),
    callback=hypothesis_callback(),
)
@settings(**standard_settings())
def test_skl_regressor(clazz, n_estimators, callback):
    """Scikit-learn regressor"""
    if clazz in [RandomForestRegressor, ExtraTreesRegressor]:
        n_targets = callback.draw(integers(min_value=1, max_value=3))
    else:
        n_targets = callback.draw(just(1))
    X, y = callback.draw(standard_regression_datasets(n_targets=just(n_targets)))
    kwargs = {"max_depth": 8, "random_state": 0}
    if clazz == HistGradientBoostingRegressor:
        kwargs["max_iter"] = n_estimators
    else:
        kwargs["n_estimators"] = n_estimators
    if clazz in [GradientBoostingRegressor, HistGradientBoostingRegressor]:
        kwargs["learning_rate"] = callback.draw(floats(min_value=0.01, max_value=1.0))
    else:
        kwargs["n_jobs"] = -1
    if clazz == GradientBoostingClassifier:
        kwargs["init"] = callback.draw(
            sampled_from([None, DummyRegressor(strategy="mean"), "zero"])
        )
    clf = clazz(**kwargs)
    clf.fit(X, y)

    tl_model = treelite.sklearn.import_model(clf)
    out_pred = treelite.gtil.predict(tl_model, X)
    if n_targets > 1:
        expected_pred = clf.predict(X)[:, :, np.newaxis]
    else:
        expected_pred = clf.predict(X).reshape((X.shape[0], 1, -1))
    np.testing.assert_almost_equal(out_pred, expected_pred, decimal=3)


@given(
    clazz=sampled_from(
        [
            RandomForestClassifier,
            ExtraTreesClassifier,
            GradientBoostingClassifier,
            HistGradientBoostingClassifier,
        ]
    ),
    dataset=standard_classification_datasets(
        n_classes=integers(min_value=2, max_value=5),
    ),
    n_estimators=integers(min_value=3, max_value=10),
    callback=hypothesis_callback(),
)
@settings(**standard_settings())
def test_skl_classifier(clazz, dataset, n_estimators, callback):
    """Scikit-learn classifier"""
    X, y = dataset
    n_classes = len(np.unique(y))
    kwargs = {"max_depth": 8, "random_state": 0}
    if clazz == HistGradientBoostingClassifier:
        kwargs["max_iter"] = n_estimators
    else:
        kwargs["n_estimators"] = n_estimators
    if clazz in [GradientBoostingClassifier, HistGradientBoostingClassifier]:
        kwargs["learning_rate"] = callback.draw(floats(min_value=0.01, max_value=1.0))
    else:
        kwargs["n_jobs"] = -1
    if clazz == GradientBoostingClassifier:
        kwargs["init"] = callback.draw(
            sampled_from([None, DummyClassifier(strategy="prior"), "zero"])
        )
    clf = clazz(**kwargs)
    clf.fit(X, y)

    tl_model = treelite.sklearn.import_model(clf)

    if clazz == GradientBoostingClassifier and kwargs["init"] == "zero":
        base_scores = tl_model.get_header_accessor().get_field("base_scores")
        expected_base_scores_shape = (clf.n_classes_ if clf.n_classes_ > 2 else 1,)
        assert base_scores.shape == expected_base_scores_shape
        np.testing.assert_equal(base_scores, np.zeros(expected_base_scores_shape))

    out_prob = treelite.gtil.predict(tl_model, X)
    expected_prob = clf.predict_proba(X)[:, np.newaxis, :]
    if (
        clazz in [GradientBoostingClassifier, HistGradientBoostingClassifier]
        and n_classes == 2
    ):
        expected_prob = expected_prob[:, :, 1:]
    np.testing.assert_almost_equal(out_prob, expected_prob, decimal=5)


@given(
    n_classes=integers(min_value=3, max_value=5),
    n_estimators=integers(min_value=3, max_value=10),
)
@settings(**standard_settings())
def test_skl_multitarget_multiclass_rf(n_classes, n_estimators):
    """Scikit-learn RF classifier with multiple outputs and classes"""
    X, y1 = make_classification(
        n_samples=1000,
        n_features=100,
        n_informative=30,
        n_classes=n_classes,
        random_state=0,
    )
    y2 = shuffle(y1, random_state=1)
    y3 = shuffle(y1, random_state=2)

    y = np.vstack((y1, y2, y3)).T

    clf = RandomForestClassifier(
        max_depth=8, n_estimators=n_estimators, n_jobs=-1, random_state=4
    )
    clf.fit(X, y)

    tl_model = treelite.sklearn.import_model(clf)
    out_prob = treelite.gtil.predict(tl_model, X)
    expected_prob = np.transpose(clf.predict_proba(X), axes=(1, 0, 2))
    np.testing.assert_almost_equal(out_prob, expected_prob, decimal=5)


@given(dataset=standard_regression_datasets())
@settings(**standard_settings())
def test_skl_converter_iforest(dataset):
    """Scikit-learn isolation forest"""
    X, _ = dataset
    clf = IsolationForest(
        max_samples=64,
        n_estimators=10,
        n_jobs=-1,
        random_state=0,
    )
    clf.fit(X)
    expected_pred = -clf.score_samples(X)
    expected_pred = expected_pred.reshape((-1, 1, 1))

    tl_model = treelite.sklearn.import_model(clf)
    out_pred = treelite.gtil.predict(tl_model, X)
    np.testing.assert_almost_equal(out_pred, expected_pred, decimal=2)


@given(
    dataset=standard_classification_datasets(
        n_classes=integers(min_value=2, max_value=4),
    ),
    num_boost_round=integers(min_value=5, max_value=20),
    use_categorical=sampled_from([True, False]),
    callback=hypothesis_callback(),
)
@settings(**standard_settings())
def test_skl_hist_gradient_boosting_with_categorical(
    dataset, num_boost_round, use_categorical, callback
):
    """Scikit-learn HistGradientBoostingClassifier, with categorical splits"""
    X, y = dataset
    if use_categorical:
        n_categorical = callback.draw(sampled_from([3, 5, X.shape[1]]))
        assume(n_categorical <= X.shape[1])
        df, X_pred = to_categorical(X, n_categorical=n_categorical, invalid_frac=0.0)
        cat_col_bitmap = df.dtypes == "category"
        categorical_features = [
            i for i in range(len(cat_col_bitmap)) if cat_col_bitmap[i]
        ]
    else:
        df = pd.DataFrame(X)
        categorical_features = None
        X_pred = X.copy()

    clf = HistGradientBoostingClassifier(
        max_iter=num_boost_round,
        categorical_features=categorical_features,
        early_stopping=False,
        max_depth=8,
    )
    clf.fit(df, y)

    tl_model = treelite.sklearn.import_model(clf)
    out_pred = treelite.gtil.predict(tl_model, X_pred)
    expected_pred = clf.predict_proba(X_pred)[:, np.newaxis, 1:]
    np.testing.assert_almost_equal(out_pred, expected_pred, decimal=4)


@pytest.mark.skipif(
    parse_version(sklearn_version) < parse_version("1.4.0"),
    reason="Requires scikit-learn 1.4.0+",
)
def test_skl_hist_gradient_boosting_with_string_categorical():
    """Scikit-learn HistGradientBoostingClassifier, with string categorical features"""
    X = np.array([["Male", 1], ["Female", 3], ["Female", 2]])
    y = [0, 1, 1]
    clf = HistGradientBoostingClassifier(
        max_iter=1,
        categorical_features=[0, 1],
        early_stopping=False,
        max_depth=1,
    )
    clf.fit(X, y)

    with pytest.raises(
        NotImplementedError, match=r"String categories are not supported"
    ):
        _ = treelite.sklearn.import_model(clf)


@given(
    dataset=standard_regression_datasets(
        n_targets=integers(min_value=1, max_value=3),
    ),
    max_depth=integers(min_value=3, max_value=15),
    n_estimators=integers(min_value=5, max_value=10),
)
@settings(**standard_settings())
def test_skl_export_rf_regressor(dataset, max_depth, n_estimators):
    """Round trip with scikit-learn RF regressor"""
    X, y = dataset
    clf = RandomForestRegressor(
        max_depth=max_depth, random_state=0, n_estimators=n_estimators, n_jobs=-1
    )
    clf.fit(X, y)

    tl_model = treelite.sklearn.import_model(clf)
    clf2 = treelite.sklearn.export_model(tl_model)
    assert isinstance(clf2, RandomForestRegressor)
    np.testing.assert_almost_equal(clf2.predict(X), clf.predict(X))


@given(
    dataset=standard_classification_datasets(
        n_classes=integers(min_value=2, max_value=4),
    ),
    max_depth=integers(min_value=3, max_value=15),
    n_estimators=integers(min_value=5, max_value=10),
)
@settings(**standard_settings())
def test_skl_export_rf_classifier(dataset, max_depth, n_estimators):
    """Round trip with scikit-learn RF classifier"""
    X, y = dataset
    clf = RandomForestClassifier(
        max_depth=max_depth, random_state=0, n_estimators=n_estimators, n_jobs=-1
    )
    clf.fit(X, y)

    tl_model = treelite.sklearn.import_model(clf)
    clf2 = treelite.sklearn.export_model(tl_model)
    assert isinstance(clf2, RandomForestClassifier)
    np.testing.assert_almost_equal(clf2.predict_proba(X), clf.predict_proba(X))


@given(
    n_classes=integers(min_value=3, max_value=5),
    n_estimators=integers(min_value=3, max_value=10),
)
@settings(**standard_settings())
def test_skl_export_rf_multitarget_multiclass(n_classes, n_estimators):
    """Round trip with scikit-learn RF classifier, with multiple outputs and classes"""
    X, y1 = make_classification(
        n_samples=1000,
        n_features=100,
        n_informative=30,
        n_classes=n_classes,
        random_state=0,
    )
    y2 = shuffle(y1, random_state=1)
    y3 = shuffle(y1, random_state=2)

    y = np.vstack((y1, y2, y3)).T

    clf = RandomForestClassifier(
        max_depth=8, n_estimators=n_estimators, n_jobs=-1, random_state=4
    )
    clf.fit(X, y)

    tl_model = treelite.sklearn.import_model(clf)
    clf2 = treelite.sklearn.export_model(tl_model)
    assert isinstance(clf2, RandomForestClassifier)
    np.testing.assert_almost_equal(clf2.predict_proba(X), clf.predict_proba(X))
