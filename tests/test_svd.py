import numpy as np
import pytest

import svd


def _reconstruct(matrix: np.ndarray) -> np.ndarray:
    u, s, v = svd.svd(matrix)
    sigma = np.diag(s)
    return u @ sigma @ v.T


def _align_columns(candidate: np.ndarray, reference: np.ndarray) -> np.ndarray:
    aligned = candidate.copy()
    for idx in range(reference.shape[1]):
        dot = np.dot(aligned[:, idx], reference[:, idx])
        if dot < 0:
            aligned[:, idx] *= -1
    return aligned


def test_reconstruction_matches_original() -> None:
    matrix = np.array(
        [
            [3.0, 1.0, 1.0],
            [-1.0, 3.0, 1.0],
            [0.0, 0.0, 2.0],
        ],
        dtype=np.float64,
    )

    rebuilt = _reconstruct(matrix)
    assert np.allclose(rebuilt, matrix, atol=1e-8)


def test_matches_numpy_svd() -> None:
    rng = np.random.default_rng(42)
    matrix = rng.normal(size=(5, 3))

    u_impl, s_impl, v_impl = svd.svd(matrix)
    u_np, s_np, vh_np = np.linalg.svd(matrix, full_matrices=False)
    v_np = vh_np.T

    assert np.allclose(s_impl, s_np, atol=1e-6)

    u_impl_aligned = _align_columns(u_impl, u_np)
    v_impl_aligned = _align_columns(v_impl, v_np)

    assert np.allclose(u_impl_aligned, u_np, atol=1e-6)
    assert np.allclose(v_impl_aligned, v_np, atol=1e-6)

    rebuilt = u_impl @ np.diag(s_impl) @ v_impl.T
    rebuilt_np = u_np @ np.diag(s_np) @ v_np.T
    assert np.allclose(rebuilt, rebuilt_np, atol=1e-6)
