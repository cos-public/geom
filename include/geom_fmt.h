#ifndef GEOM_FMT_H
#define GEOM_FMT_H

#ifndef GEOM_H
#error geom.h must be included first
#endif

#include <fmt/format.h>

namespace fmt {

using namespace geom;

template <typename T>
struct formatter<point<T>> {
	template <typename ParseContext>
	constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(const point<T> & pt, FormatContext &ctx) {
		return format_to(ctx.out(), FMT_STRING("({}, {})"), pt.x, pt.y);
	}
};

template <typename T>
struct formatter<size<T>> {
	template <typename ParseContext>
	constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(const size<T> & sz, FormatContext &ctx) {
		/// \u00D7 == \xC3\x97
		return format_to(ctx.out(), FMT_STRING("[{}" "\xC3\x97" "{}]"), sz.width, sz.height);
	}
};

template <typename T, typename S>
struct formatter<rect<T, S>> {
	template <typename ParseContext>
	constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(const rect<T, S> & r, FormatContext &ctx) {
		return format_to(ctx.out(), FMT_STRING("{} {} {}"), r.top_left(), r.bottom_right(), r.size());
	}
};

} //ns fmt

#endif //GEOM_FMT_H
