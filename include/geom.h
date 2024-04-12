#ifndef GEOM_H
#define GEOM_H

#if defined _WIN32
#include <Windows.h> /// for POINT, POINTF, SIZE
#endif
#include <algorithm> /// for minmax
#include <memory> /// for unique_ptr
#include <numbers> /// for pi
#include <optional>
#include <cmath> /// for lroundf
#include <stdexcept>
#include <bit> /// for countl_zero

/// point, size, rect classes

namespace geom {

template <std::floating_point T>
[[nodiscard]] inline constexpr T deg2rad(T angle) {
	constexpr T k = std::numbers::pi_v<T> / T{180.0};
	return angle * k;
}

template <std::floating_point T>
[[nodiscard]] inline constexpr T rad2deg(T angle) {
	constexpr T k = T{180.0} / std::numbers::pi_v<T>;
	return angle * k;
}


template <typename T>
struct point {
	using value_type = T;

	T x, y;
#ifdef _WIN32
	/// to keep the class aggregate do not introduce a constructor
	static constexpr inline point from_POINT(const POINT & pt) {
		static_assert(std::is_integral_v<T>, "Narrowing conversion");
		return { T{pt.x}, T{pt.y} };
	}
	explicit constexpr inline operator POINT() const { return POINT{x, y}; }
#endif //_WIN32
	[[nodiscard]] inline constexpr bool operator!=(const point<T> & rhs) const { return x != rhs.x || y != rhs.y; }
	[[nodiscard]] inline constexpr bool operator==(const point<T> & rhs) const { return !(*this != rhs); }
	[[nodiscard]] inline constexpr point<T> operator+(const point<T> & rhs) const { return point<T>{ x + rhs.x, y + rhs.y }; }
	[[nodiscard]] inline constexpr point<T> operator-(const point<T> & rhs) const { return point<T>{ x - rhs.x, y - rhs.y }; }
	[[nodiscard]] inline constexpr point<T> operator*(const point<T> & rhs) const { return point<T>{ x * rhs.x, y * rhs.y }; }
	[[nodiscard]] inline constexpr point<T> operator/(const point<T> & rhs) const { return point<T>{ x / rhs.x, y / rhs.y }; }
	[[nodiscard]] inline constexpr point<T> operator-() const { return point<T>{-x, -y}; } /// unary minus
	inline constexpr point<T> & operator+=(const point<T> & rhs) { x += rhs.x; y += rhs.y; return *this; }
	inline constexpr point<T> & operator+=(const T rhs) { x += rhs; y += rhs; return *this; }
	inline constexpr point<T> & operator-=(const point<T> & rhs) { x -= rhs.x; y -= rhs.y; return *this; }
	inline constexpr point<T> & operator-=(const T rhs) { x -= rhs; y -= rhs; return *this; }
	inline constexpr point<T> & operator*=(const point<T> & rhs) { x *= rhs.x; y *= rhs.y; return *this; }
	inline constexpr point<T> & operator*=(const T rhs) { x *= rhs; y *= rhs; return *this; }
	inline constexpr point<T> & operator/=(const point<T> & rhs) { x /= rhs.x; y /= rhs.y; return *this; }
	inline constexpr point<T> & operator/=(const T rhs) { x /= rhs; y /= rhs; return *this; }
	template <typename U>
	point<U> cast() const { return point<U>{ .x = static_cast<U>(x), .y = static_cast<U>(y)}; }
	template <typename U>
	point<U> round() const;
	[[nodiscard]] T manhattan_length() const {
		const auto & [min, max] = std::minmax(x, y);
		return max - min;
	}
	point<T> & rotate(float angle_rad) {
		const float c = std::cos(angle_rad);
		const float s = std::sin(angle_rad);
		x = x * c - y * s;
		y = y * c + x * s;
		return *this;
	}
};

template <> template <>
inline point<int> point<float>::round() const {
	return point<int>{ static_cast<int>(std::lroundf(x)), static_cast<int>(std::lroundf(y)) };
}

template <typename T>
[[nodiscard]] inline constexpr point<T> operator/(const T lhs, const point<T> & rhs) {
	return point<T>{lhs / rhs.x, lhs / rhs.y};
}

template <typename T>
[[nodiscard]] inline constexpr point<T> operator+(point<T> lhs, const T & rhs) {
	lhs += rhs;
	return lhs;
}

template <typename T>
[[nodiscard]] inline constexpr point<T> operator-(point<T> lhs, const T & rhs) {
	lhs -= rhs;
	return lhs;
}

template <typename T>
[[nodiscard]] inline constexpr point<T> operator*(point<T> lhs, const T & rhs) {
	lhs *= rhs;
	return lhs;
}

template <typename T>
[[nodiscard]] inline constexpr point<T> operator/(point<T> lhs, const T & rhs) {
	lhs /= rhs;
	return lhs;
}

#ifdef _WIN32

/// usage: ::ScreenToClient(*this, geom::pPOINT_adapter(pt));
template <typename T>
class pPOINT_adapter {
public:
	pPOINT_adapter(point<T> & p) : p(p), pp{ p.x, p.y } {
	}

	~pPOINT_adapter() {
		p.x = pp.x;
		p.y = pp.y;
	}

	operator POINT *() {
		return &pp;
	}

private:
	point<T> & p;
	POINT pp;
};
#endif //_WIN32


using pointi = point<int>;
using pointu = point<unsigned int>;
using pointf = point<float>;


template <typename T>
struct size {
public:
	using value_type = T;
	T width, height;

	/// to keep the class aggregate do not introduce a constructor
	static constexpr inline size from_array(const std::array<T, 2> & arr) {
		return size{ .width = std::get<0>(arr), .height = std::get<1>(arr) };
	}
#ifdef _WIN32
	static constexpr inline size from_SIZE(const SIZE & sz) {
		return { .width = static_cast<T>(sz.cx), .height = static_cast<T>(sz.cy) };
	}
	explicit constexpr inline operator SIZE() const {
		return SIZE{ width, height };
	}
#endif
	template<typename U>
	size<U> cast() const { return size<U>{ static_cast<U>(width), static_cast<U>(height) }; }

	[[nodiscard]] static size<T> from_point(point<T> pt) { return size<T>{pt.x, pt.y}; };
	[[nodiscard]] point<T> to_point() const { return point<T>{ width, height }; };

	inline constexpr size<T> & operator+=(const size<T> & rhs) { width += rhs.width; height += rhs.height; return *this; }
	inline constexpr size<T> & operator+=(T rhs) { width += rhs; height += rhs; return *this; }
	inline constexpr size<T> & operator-=(const size<T> & rhs) { width -= rhs.width; height -= rhs.height; return *this; }
	inline constexpr size<T> & operator-=(T rhs) { width -= rhs; height -= rhs; return *this; }
	template<typename U>
	inline constexpr size<T> & operator*=(const U & mul) {
		static_assert(std::is_arithmetic_v<U>);
		width *= mul;
		height *= mul;
		return *this;
	}
	inline constexpr size<T> & operator/=(const T & divider) { width /= divider; height /= divider; return *this; }

	template <typename U>
	size<U> round() const;

	[[nodiscard]] inline size<T> fitted(size<T> bounds) const {
		const float zw = (float) bounds.width / width;
		const float zh = (float) bounds.height / height;
		if (zw < zh) {
			return {bounds.width, (T) (height * zw)};
		} else {
			return {(T) (width * zh), bounds.height};
		}
	}
};

template <> template <>
inline size<unsigned> size<float>::round() const {
	return size<unsigned>{ static_cast<unsigned>(std::lroundf(width)), static_cast<unsigned>(std::lroundf(height)) };
}


template<typename T>
[[nodiscard]] inline size<T> operator+ (size<T> lhs, const size<T> & rhs) { lhs += rhs; return lhs; }
template<typename T>
[[nodiscard]] inline size<T> operator+ (size<T> lhs, T rhs) { lhs += rhs; return lhs; }
template<typename T>
[[nodiscard]] inline size<T> operator- (size<T> lhs, const size<T> & rhs) { lhs -= rhs; return lhs; }
template<typename T>
[[nodiscard]] inline size<T> operator- (size<T> lhs, T rhs) { lhs -= rhs; return lhs; }
template<typename T, typename U>
[[nodiscard]] inline size<T> operator* (size<T> lhs, const U & rhs) { lhs *= rhs; return lhs; }
template<typename T>
[[nodiscard]] inline size<T> operator/ (size<T> lhs, T divider) { lhs /= divider; return lhs; }

template <typename T>
inline constexpr bool operator==(const size<T> & lhs, const size<T> & rhs) {
	return lhs.width == rhs.width && lhs.height == rhs.height;
}

template <typename T>
inline constexpr bool operator!=(const size<T> & lhs, const size<T> & rhs) {
	return lhs.width != rhs.width || lhs.height != rhs.height;
}

template <typename T>
inline constexpr pointf scale_factor(const size<T> & numer, const size<T> & denom) {
	return pointf{
		(float) numer.width / (float) denom.width,
		(float) numer.height / (float) denom.height
	};
}


using sizei = size<int>;
using sizeu = size<unsigned int>;
using sizef = size<float>;


template <typename T, typename S = T>
class rect {
public:
	using value_type = T;
	constexpr rect(const rect &) = default;
	constexpr rect(T x1, T y1, T x2, T y2) : x1(x1), y1(y1), x2(x2), y2(y2) {
		if constexpr (std::is_unsigned_v<S>) {
			if (x2 < x1 || y2 < y1) {
				throw std::invalid_argument("Negative unsigned rect dimension");
			}
		}
	}
#ifdef _WIN32
	constexpr rect(const RECT & rect) : x1(rect.left), y1(rect.top), x2(rect.right), y2(rect.bottom) {
		if constexpr (std::is_unsigned_v<S>) {
			if (x2 < x1 || y2 < y1) {
				throw std::invalid_argument("Negative unsigned rect dimension");
			}
		}
	}
#endif
	constexpr rect(const point<T> & org, const size<S> & size) noexcept : x1(org.x), y1(org.y), x2(org.x + static_cast<T>(size.width)), y2(org.y + static_cast<T>(size.height)) {}
	constexpr rect(const point<T> & org, const point<T> & dest) noexcept : x1(org.x), y1(org.y), x2(dest.x), y2(dest.y) {}
	explicit constexpr rect(const size<S> & size) noexcept : rect(point<T>{0, 0}, size) {}
	static rect<T, S> from_size(T x, T y, S w, S h) { return rect<T, S>(x, y, x+w, y+h); }
	[[nodiscard]] inline constexpr S width() const noexcept { return static_cast<S>(x2 - x1); }
	inline constexpr void set_width(S width) noexcept { x2 = x1 + width; }
	[[nodiscard]] inline S height() const noexcept { return static_cast<S>(y2 - y1); }
	inline void set_height(S height) noexcept { y2 = y1 + height; }
	inline constexpr void move_left(T x) noexcept { x2 = x + (x2 - x1); x1 = x; }
	inline constexpr void move_top(T y) noexcept { y2 = y + (y2 - y1); y1 = y; }
	inline constexpr void move_right(T x) noexcept { x1 = x - (x2 - x1); x2 = x; }
	inline constexpr void move_bottom(T y) noexcept { y1 = y - (y2 - y1); y2 = y; }
	inline constexpr void move(T x, T y) noexcept { x2 = x + (x2 - x1); y2 = y + (y2 - y1); x1 = x; y1 = y; }
	inline constexpr void move(const point<T> & org) noexcept { move(org.x, org.y); }
	inline constexpr rect<T, S> & move_center(T cx, T cy) noexcept {
		const auto w = width(); const auto h = height();
		x1 = cx - w / 2; y1 = cy - h / 2;
		x2 = x1 + w; y2 = y1 + h;
		return *this;
	}
	inline constexpr rect<T, S> & move_center(const point<T> c) noexcept { return move_center(c.x, c.y); }
	inline constexpr void resize(const size<S> & size) noexcept { x2 = x1 + size.width; y2 = y1 + size.height; }
	[[nodiscard]] inline constexpr T top() const noexcept { return y1; }
	[[nodiscard]] inline constexpr T left() const noexcept { return x1; }
	[[nodiscard]] inline constexpr T bottom() const noexcept { return y2; }
	[[nodiscard]] inline constexpr T right() const noexcept { return x2; }
	inline constexpr T & rtop() noexcept { return y1; }
	inline constexpr T & rleft() noexcept { return x1; }
	inline constexpr T & rbottom() noexcept { return y2; }
	inline constexpr T & rright() noexcept { return x2; }
	[[nodiscard]] inline constexpr point<T> org() const noexcept { return top_left(); }
	[[nodiscard]] inline constexpr point<T> dest() const noexcept { return bottom_right(); }
	[[nodiscard]] inline constexpr point<T> top_left() const noexcept { return point<T>{x1, y1}; }
	[[nodiscard]] inline constexpr point<T> top_right() const noexcept { return point<T>{x2, y1}; }
	[[nodiscard]] inline constexpr point<T> bottom_left() const noexcept { return point<T>{x1, y2}; }
	[[nodiscard]] inline constexpr point<T> bottom_right() const noexcept { return point<T>{x2, y2}; }
	[[nodiscard]] inline constexpr geom::size<S> size() const noexcept { return geom::size<S>{ width(), height() }; }
	[[nodiscard]] inline constexpr point<T> center() const noexcept {
		return point<T>{
			x1 + static_cast<T>(x2 - x1) / T{2},
			y1 + static_cast<T>(y2 - y1) / T{2}
		};
	}
	[[nodiscard]] inline constexpr bool empty() const noexcept { return x2 == x1 || y2 == y1; }
	[[nodiscard]] inline constexpr bool contains(T x, T y) const noexcept { return x1 <= x && x < x2 && y1 <= y && y < y2; }
	[[nodiscard]] inline constexpr bool contains(const point<T> & pt) const noexcept { return contains(pt.x, pt.y); }
	[[nodiscard]] inline constexpr bool contains(const geom::rect<T, S> & inner) const noexcept {
		return inner.x1 >= x1 && inner.y1 >= y1 && inner.x2 <= x2 && inner.y2 <= y2;
	}
	[[nodiscard]] inline constexpr rect<T, S> translated(T dx, T dy) const noexcept {
		return rect<T, S>(x1 + dx, y1 + dy, x2 + dx, y2 + dy);
	}
	[[nodiscard]] inline constexpr rect<T, S> translated(const point<T> & dt) const noexcept { return this->translated(dt.x, dt.y); }
	inline constexpr void translate(T dx, T dy) noexcept { *this = translated(dx, dy); }
	inline constexpr void translate(const point<T> & dt) noexcept { *this = translated(dt); }
	[[nodiscard]] inline constexpr rect<T, S> adjusted(T dx1, T dy1, T dx2, T dy2) const noexcept {
		return rect<T, S>(x1 + dx1, y1 + dy1, x2 + dx2, y2 + dy2);
	}
	inline constexpr void adjust(T dx1, T dy1, T dx2, T dy2) noexcept { *this = adjusted(dx1, dy1, dx2, dy2); }
	[[nodiscard]] inline constexpr rect<T, S> expanded(T d) const noexcept {
		return rect<T, S>(x1 - d, y1 - d, x2 + d, y2 + d);
	}
	[[nodiscard]] inline constexpr rect<T, S> shrinked(T d) const noexcept { return expanded(-d); }
	[[nodiscard]] inline constexpr rect<T, S> united(const rect<T, S> & other) const {
		if (empty())
			return other;
		if (other.empty())
			return *this;
		return rect<T, S>{std::min(x1, other.x1), std::min(y1, other.y1), std::max(x2, other.x2), std::max(y2, other.y2)};
	}
	inline constexpr void unite(const rect<T, S> & other) { *this = united(other); }
	/// intersected could be used instead
	//[[nodiscard]] inline constexpr bool overlaps(const rect<T, S> & other) const noexcept {
	//	return !(left() >= other.right() || right() <= other.left() || top() >= other.bottom() || bottom() <= other.top());
	//}
	template <typename U>
	[[nodiscard]] inline constexpr rect<T> scaled(U num, U denom) const noexcept {
		static_assert(std::is_integral<T>::value && std::is_integral<U>::value, "Integer required.");
		return rect<T>(x1 * num / denom, y1 * num / denom, x2 * num / denom, y2 * num / denom);
	}
	inline constexpr void scale(float f, const pointi & c) {
		x1 = c.x - (T) std::round((c.x - x1) * f);
		x2 = c.x + (T) std::round((x2 - c.x) * f);
		y1 = c.y - (T) std::round((c.y - y1) * f);
		y2 = c.y + (T) std::round((y2 - c.y) * f);
	}
	void transpose() noexcept {
		std::swap(x1, y1);
		std::swap(x2, y2);
	}
	[[nodiscard]] inline constexpr rect<T, S> transposed() const noexcept {
		auto r = *this;
		r.transpose();
		return r;
	}
	[[nodiscard]] inline constexpr std::optional<rect<T, S>> intersected(const rect<T, S> & other) const {
		if (other.x1 >= x2 || other.x2 <= x1 || other.y1 >= y2 || other.y2 <= y1)
			return std::nullopt;
		return rect<T, S>(std::max(x1, other.x1), std::max(y1, other.y1),
			std::min(x2, other.x2), std::min(y2, other.y2));
	}
#ifdef _WIN32
	[[nodiscard]] explicit inline constexpr operator RECT() const noexcept { return { x1, y1, x2, y2 }; }
	/// for API which requires const RECT *
	/// usage like: ::MonitorFromRect(window_rect.pRECT().get(), MONITOR_DEFAULTTONULL);
	[[nodiscard]] std::unique_ptr<const RECT> pRECT() const { return std::make_unique<const RECT>(*this); }
#endif //_WIN32
	template<typename T2, typename S2 = T2>
	[[nodiscard]] rect<T2, S2> cast() const {
		return rect<T2, S2>{static_cast<T2>(x1), static_cast<T2>(y1), static_cast<T2>(x2), static_cast<T2>(y2)};
	}

	[[nodiscard]] rect<T, S> operator-(const geom::size<S> & sz) { return rect<T, S>(x1, y1, x2 - sz.width(), y2 - sz.height()); }

	[[nodiscard]] friend inline constexpr bool operator==(const rect<T, S> & lhs, const rect<T, S> & rhs) {
		return lhs.x1 == rhs.x1 && lhs.y1 == rhs.y1 && lhs.x2 == rhs.x2 && lhs.y2 == rhs.y2;
	}

private:
	T x1, y1, x2, y2;
};

#ifdef _WIN32

/// usage: DrawText(dc, "Hello", -1, geom::pRECT_adapter(rc), 0);
template <typename T, typename S>
class pRECT_adapter {
public:
	pRECT_adapter(rect<T, S> & rc) : rc{rc}, rrc{ rc.left(), rc.top(), rc.right(), rc.bottom() } {
	}

	~pRECT_adapter() {
		rc.rleft() = rrc.left;
		rc.rtop() = rrc.top;
		rc.rright() = rrc.right;
		rc.rbottom() = rrc.bottom;
	}

	operator RECT *() {
		return &rrc;
	}

private:
	rect<T, S> & rc;
	RECT rrc;
};
#endif //_WIN32


template <typename T, typename S>
inline std::optional<rect<T, S>> intersect(const std::optional<rect<T, S>> & a, const rect<T, S> & b) {
	if (!a.has_value())
		return b;
	return a->intersected(b);
}

template <typename T, typename S>
inline std::optional<rect<T, S>> intersect(const std::optional<rect<T, S>> & a, const std::optional<rect<T, S>> & b) {
	if (!a.has_value()) {
		return b;
	}
	if (!b.has_value()) {
		return a;
	}

	return a->intersected(b);
}

template <typename T, typename S>
inline rect<T, S> unite(const std::optional<rect<T, S>> & a, const rect<T, S> & b) {
	if (!a.has_value())
		return b;
	return a->united(b);
}

template <typename T, typename S>
[[nodiscard]] inline rect<T, S> fit_rect(geom::size<S> sz, geom::rect<T, S> bounds) {
	const auto fitted_sz = sz.fitted(bounds.size());
	const geom::point<T> org{bounds.left() + (T) (bounds.width() - fitted_sz.width) / 2, bounds.top() + (T) (bounds.height() - fitted_sz.height) / 2};
	return geom::rect<T, S>{org, fitted_sz};
}


template <typename T, typename U>
point<T> clamp(point<T> pt, rect<T, U> bounds) {
	if (pt.x < bounds.left()) pt.x = bounds.left();
	if (pt.x > bounds.right()) pt.x = bounds.right();
	if (pt.y < bounds.top()) pt.y = bounds.top();
	if (pt.y > bounds.bottom()) pt.y = bounds.bottom();
	return pt;
}


enum class orientation { vert, hor };

template <orientation O>
struct ortho_s;
template <>
struct ortho_s<orientation::vert> { static constexpr orientation o = orientation::hor; };
template <>
struct ortho_s<orientation::hor> { static constexpr orientation o = orientation::vert; };

template <orientation O>
inline constexpr orientation orthogonal = ortho_s<O>::o;

template <std::unsigned_integral T>
[[nodiscard]] inline unsigned mip_levels(const size<T> & base_size) noexcept {
	static_assert(std::is_unsigned_v<T>, "Must be unsigned");
	return (sizeof(T) << 3) - static_cast<unsigned>(std::countl_zero(std::min(base_size.width, base_size.height)));
}

template <std::unsigned_integral T>
[[nodiscard]] inline unsigned mip_levels(const size<T> & base_size, unsigned trim_levels) noexcept {
	auto l = mip_levels(base_size);
	return (l > trim_levels + 1u) ? (l - trim_levels) : 1u;
}

template <std::unsigned_integral T>
[[nodiscard]] inline size<T> mip_size(const size<T> & base_size, unsigned level) {
	return size<T>{ .width = base_size.width / (1u << level),
		.height = base_size.height / (1u << level) };
}

/// nearest mip level to be minified
template <std::unsigned_integral T>
[[nodiscard]] constexpr inline unsigned nearest_mip_level(const size<T> & base_size, const size<T> request_size) {
	if (request_size.width >= base_size.width || request_size.height >= base_size.height)
		return 0u;
	const uint32_t z = std::min(base_size.width / request_size.width,
		base_size.height / request_size.height);
	//assert(std::countl_zero(z) < 32u);
	return 32u - std::countl_zero(z) - 1u;
}



using recti = rect<int>;
using rectu = rect<unsigned int>;
using rectf = rect<float>;
using rectn = rect<int, unsigned>; /// normalized rect

} //ns geom

#endif //GEOM_H