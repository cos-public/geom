# A simple geometry library
Provides templated `point<>` (`pointu`, `pointi`, `pointf`), `size<>` (`sizeu`, `sizei`, `sizef`) and `rect<>` (`rectu`, `recti`, `rectf`, `rectn`) classes with specializations for `unsigned`, `int`, `float`.

`rectn` is a specialization for normalized rect with `int` coordinate of origin and `unsigned` size.

# Design choices
* `point` and `size` are plain `struct`s to allow struct initialization.
* Use modern ``[[nodiscard]]``, `constexpr`, `noexcept`, `static_assert` where appropriate.
* Do not use implicit casts
* Store `rect` as `{org, dest}` opposed to `{org, size}`
