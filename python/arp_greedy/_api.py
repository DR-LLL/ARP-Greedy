from __future__ import annotations

import math
from collections.abc import Callable, Iterable, Iterator
from dataclasses import dataclass
from typing import Generic, Literal, SupportsFloat, TypeVar

from . import _native

T = TypeVar("T")
Arithmetic = Literal["filtered", "exact"]


@dataclass(frozen=True, slots=True)
class Job:
    """Optional convenience type; ordinary pairs work equally well."""

    processing_time: float
    weight: float

    def __iter__(self) -> Iterator[float]:
        yield self.processing_time
        yield self.weight


@dataclass(frozen=True, slots=True)
class Statistics:
    predicates: int
    exact_fallbacks: int
    bridge_steps: int
    query_nodes: int
    repaired_nodes: int


@dataclass(frozen=True, slots=True)
class Schedule(Generic[T]):
    """Indices refer to input iteration order; original objects are not copied."""

    order: tuple[int, ...]
    ordered_jobs: tuple[T, ...]
    objective: float
    objective_overflow: bool
    statistics: Statistics

    @property
    def departure_order(self) -> tuple[int, ...]:
        """Aircraft departure indices (reverse of job execution order)."""
        return self.order[::-1]

    @property
    def departure_jobs(self) -> tuple[T, ...]:
        return self.ordered_jobs[::-1]


def schedule(
    jobs: Iterable[T],
    *,
    processing_time: Callable[[T], SupportsFloat] | None = None,
    weight: Callable[[T], SupportsFloat] | None = None,
    arithmetic: Arithmetic = "filtered",
) -> Schedule[T]:
    """Consume a finite iterable once and schedule it with the maximum-phi rule.

    By default each item is an iterable pair (processing_time, weight).
    For arbitrary objects, provide BOTH field-extraction callables. Extraction
    happens once per field per item, in Python, before the GIL-released C++ call.

    Numbers are converted to float (binary64), then scheduling comparisons are
    exact for those represented values. Input indices follow iteration order.
    Ties use smaller exact processing_time/weight, then input index. Zero-weight
    jobs are appended in input order. The input container is never rearranged.

    The iterable is materialized: this is an offline algorithm, not a streaming
    online scheduler. The result retains references to the original objects.
    Mutating them later does not recompute the returned order or objective.
    """
    get_time, get_weight = processing_time, weight
    if arithmetic not in ("filtered", "exact"):
        raise ValueError("arithmetic must be 'filtered' or 'exact'")
    if (get_time is None) != (get_weight is None):
        raise TypeError("Provide both processing_time and weight extractors, or neither")
    if get_time is not None and (not callable(get_time) or not callable(get_weight)):
        raise TypeError("processing_time and weight must be callable")
    originals: list[T] = []
    pairs: list[tuple[float, float]] = []
    for index, item in enumerate(jobs):
        if get_time is None:
            if isinstance(item, (str, bytes, bytearray)):
                raise TypeError(f"Job {index} must be a (processing_time, weight) pair, not text")
            try:
                p, w = item  # type: ignore[misc]
            except (TypeError, ValueError) as error:
                raise TypeError(
                    f"Job {index} must be a pair; for objects provide processing_time and weight"
                ) from error
        else:
            p = get_time(item)
            assert get_weight is not None
            w = get_weight(item)
        try:
            p, w = float(p), float(w)
        except (TypeError, ValueError, OverflowError) as error:
            raise ValueError(f"Job {index} has a value that cannot be converted to binary64") from error
        if not math.isfinite(p) or p <= 0:
            raise ValueError(f"Job {index}: processing_time must be finite and positive")
        if not math.isfinite(w) or w < 0:
            raise ValueError(f"Job {index}: weight must be finite and nonnegative")
        originals.append(item)
        pairs.append((p, w))
    order_list, objective, overflow, counters = _native.solve(pairs, arithmetic)
    order = tuple(order_list)
    return Schedule(
        order=order,
        ordered_jobs=tuple(originals[i] for i in order),
        objective=objective,
        objective_overflow=overflow,
        statistics=Statistics(**counters),
    )
