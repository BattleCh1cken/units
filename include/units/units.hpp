#pragma once

#include <cmath>
#include <ratio>
#include <iostream>

// define M_PI if not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @brief Quantity class
 *
 * This class is a template class that represents a quantity with a value and units.
 *
 * @tparam TYPENAMES the types of the units
 */
template <typename Mass = std::ratio<0>, typename Length = std::ratio<0>, typename Time = std::ratio<0>,
          typename Current = std::ratio<0>, typename Angle = std::ratio<0>, typename Temperature = std::ratio<0>,
          typename Luminosity = std::ratio<0>, typename Moles = std::ratio<0>>
class Quantity {
    protected:
        /** the value stored in its base unit type */
        double value;
    public:
        /** mass unit type */
        typedef Mass mass;
        /** length unit type */
        typedef Length length;
        /** time unit type */
        typedef Time time;
        /** current unit type */
        typedef Current current;
        /** angle unit type */
        typedef Angle angle;
        /** temperature unit type */
        typedef Temperature temperature;
        /** luminosity unit type */
        typedef Luminosity luminosity;
        /** moles unit type */
        typedef Moles moles;

        /**
         * @brief convenience label. Represents an equivalent unit (equal dimensions)
         */
        using Self = Quantity<Mass, Length, Time, Current, Angle, Temperature, Luminosity, Moles>;

        /**
         * @brief construct a new Quantity object
         *
         * This constructor initializes the value to 0
         */
        explicit constexpr Quantity() : value(0) {}

        /**
         * @brief construct a new Quantity object
         *
         * @param value the value to initialize the quantity with
         */
        explicit constexpr Quantity(double value) : value(value) {}

        /**
         * @brief construct a new Quantity object
         *
         * @param other the quantity to copy
         */
        constexpr Quantity(Self const& other) : value(other.value) {}

        /**
         * @brief get the value of the quantity in its base unit type. Not recommended for users
         *
         * @return the value in the base unit
         */
        constexpr double internal() const { return value; }

        /**
         * @brief get the value of the quantity in a specific unit. Not recommended for users, especially for units
         * involving temperatures or angles
         *
         * @param quantity the unit value to convert into
         * @return the value in the specified unit
         */
        constexpr double convert(Self quantity) { return value / quantity.value; }

        /**
         * @brief set the value of this quantity to its current value plus another quantity
         *
         * @param other the quantity to add
         */
        constexpr void operator+=(Self other) { value += other.value; }

        /**
         * @brief set the value of this quantity to its current value minus another quantity
         *
         * @param other the quantity to subtract
         */
        constexpr void operator-=(Self other) { value -= other.value; }

        /**
         * @brief set the value of this quantity to its current value times a double
         *
         * @param multiple the multiple to multiply by
         */
        constexpr void operator*=(double multiple) { value *= multiple; }

        /**
         * @brief set the value of this quantity to its current value divided by a double
         *
         * @param dividend the dividend to divide by
         */
        constexpr void operator/=(double dividend) { value /= dividend; }

        /**
         * @brief set the value of this quantity to a double, only if the quantity is a number
         *
         * @param rhs the double to assign
         */
        constexpr void operator=(const double& rhs) {
            static_assert(std::ratio_equal<mass, std::ratio<0>>() && std::ratio_equal<length, std::ratio<0>>() &&
                              std::ratio_equal<time, std::ratio<0>>() && std::ratio_equal<current, std::ratio<0>>() &&
                              std::ratio_equal<angle, std::ratio<0>>() &&
                              std::ratio_equal<temperature, std::ratio<0>>() &&
                              std::ratio_equal<luminosity, std::ratio<0>>() && std::ratio_equal<moles, std::ratio<0>>(),
                          "Cannot assign a double directly to a non-number unit type");
            value = rhs;
        }
};

template <typename Q> struct LookupName {
        using Named = Q;
};

template <typename Q> using Named = typename LookupName<Q>::Named;

/**
 * @brief Quantity checker function for isQuantity concept. Never to be used by users
 *
 */
template <typename Mass = std::ratio<0>, typename Length = std::ratio<0>, typename Time = std::ratio<0>,
          typename Current = std::ratio<0>, typename Angle = std::ratio<0>, typename Temperature = std::ratio<0>,
          typename Luminosity = std::ratio<0>, typename Moles = std::ratio<0>>
void quantityChecker(Quantity<Mass, Length, Time, Current, Angle, Temperature, Luminosity, Moles> q) {}

/**
 * @brief concept to use in template arguments (EX: template <isQuantity Q>). Requires any instance of the type Q to
 * inherit from Quantity and have the same dimensions
 */
template <typename Q>
concept isQuantity = requires(Q q) { quantityChecker(q); };

/**
 * @brief concept to ensure unit equivalecy (equal dimensions)
 * @tparam Q the first unit type to check
 * @tparam Quantities additional (minimum of one) unit types to check
 */
template <typename Q, typename... Quantities>
concept Isomorphic = ((std::convertible_to<Q, Quantities> && std::convertible_to<Quantities, Q>) && ...) &&
                     requires(Q q) { quantityChecker(q); };

/**
 * @brief Un(type)safely coerce the a unit into a different unit
 *
 * @tparam Q1 the unit type to return
 * @tparam Q2 the unit type of the input
 * @param quantity the input quantity
 * @return the casted value
 */
template <isQuantity Q1, isQuantity Q2> constexpr inline Q1 unit_cast(Q2 quantity) { return Q1(quantity.internal()); }

/**
 * @brief simplification of unit multiplication. Multiplied<Q1, Q2> is an equivalent (as defined by Isomorphic) unit
 * type to the product of Q1 * Q2
 *
 * @tparam Q1 the multiplicand
 * @tparam Q2 the multiplier
 */
template <isQuantity Q1, isQuantity Q2> using Multiplied = Named<Quantity<
    std::ratio_add<typename Q1::mass, typename Q2::mass>, std::ratio_add<typename Q1::length, typename Q2::length>,
    std::ratio_add<typename Q1::time, typename Q2::time>, std::ratio_add<typename Q1::current, typename Q2::current>,
    std::ratio_add<typename Q1::angle, typename Q2::angle>,
    std::ratio_add<typename Q1::temperature, typename Q2::temperature>,
    std::ratio_add<typename Q1::luminosity, typename Q2::luminosity>,
    std::ratio_add<typename Q1::moles, typename Q2::moles>>>;

/**
 * @brief simplification of unit division. Divided<Q1, Q2> is an equivalent (as defined by Isomorphic) unit type to the
 * quotient of Q1 / Q2
 *
 * @tparam Q1 the dividend
 * @tparam Q2 the divisor
 */
template <isQuantity Q1, isQuantity Q2> using Divided =
    Named<Quantity<std::ratio_subtract<typename Q1::mass, typename Q2::mass>,
                   std::ratio_subtract<typename Q1::length, typename Q2::length>,
                   std::ratio_subtract<typename Q1::time, typename Q2::time>,
                   std::ratio_subtract<typename Q1::current, typename Q2::current>,
                   std::ratio_subtract<typename Q1::angle, typename Q2::angle>,
                   std::ratio_subtract<typename Q1::temperature, typename Q2::temperature>,
                   std::ratio_subtract<typename Q1::luminosity, typename Q2::luminosity>,
                   std::ratio_subtract<typename Q1::moles, typename Q2::moles>>>;

/**
 * @brief simplification of unit exponentiation. Exponentiated<Q, R> is an equivalent (as defined by Isomorphic) unit
 * type to the R'th power of Q
 *
 * @tparam Q the base
 * @tparam R the power
 */
template <isQuantity Q, typename R> using Exponentiated =
    Named<Quantity<std::ratio_multiply<typename Q::mass, R>, std::ratio_multiply<typename Q::length, R>,
                   std::ratio_multiply<typename Q::time, R>, std::ratio_multiply<typename Q::current, R>,
                   std::ratio_multiply<typename Q::angle, R>, std::ratio_multiply<typename Q::temperature, R>,
                   std::ratio_multiply<typename Q::luminosity, R>, std::ratio_multiply<typename Q::moles, R>>>;

/**
 * @brief simplification of unit exponentiation. Exponentiated<Q, R> is an equivalent (as defined by Isomorphic) unit
 * type to the R'th root of Q
 *
 * @tparam Q the radicand
 * @tparam R the degree
 */
template <isQuantity Q, typename R> using Rooted =
    Named<Quantity<std::ratio_divide<typename Q::mass, R>, std::ratio_divide<typename Q::length, R>,
                   std::ratio_divide<typename Q::time, R>, std::ratio_divide<typename Q::current, R>,
                   std::ratio_divide<typename Q::angle, R>, std::ratio_divide<typename Q::temperature, R>,
                   std::ratio_divide<typename Q::luminosity, R>, std::ratio_divide<typename Q::moles, R>>>;

/**
 * @brief adds two isomorphic (equal dimensions) quantities
 *
 * @param lhs the first addend
 * @param rhs the second addend
 * @return constexpr Q the sum
 */
template <isQuantity Q, isQuantity R> constexpr Q operator+(Q lhs, R rhs)
    requires Isomorphic<Q, R>
{
    return Q(lhs.internal() + rhs.internal());
}

/**
 * @brief subtracts two isomorphic (equal dimensions) quantities
 *
 * @param lhs the left hand minuend
 * @param rhs the right hand minuend
 * @return the difference
 */
template <isQuantity Q, isQuantity R> constexpr Q operator-(Q lhs, R rhs)
    requires Isomorphic<Q, R>
{
    return Q(lhs.internal() + rhs.internal());
}

/**
 * @brief multiplies a unit quantity with a constant numerical factor
 *
 * @param quantity the multiplicand (united)
 * @param multiple the multiplier (numeric)
 * @return the product
 */
template <isQuantity Q> constexpr Q operator*(Q quantity, double multiple) { return Q(quantity.internal() * multiple); }

/**
 * @brief multiplies a unit quantity with a constant numerical factor
 *
 * @param multiple the multiplicand (numeric)
 * @param quantity the the multiplier (united)
 * @return the product
 */
template <isQuantity Q> constexpr Q operator*(double multiple, Q quantity) { return Q(quantity.internal() * multiple); }

/**
 * @brief divides a unit quantity by a constant numerical factor
 *
 * @param quantity the dividend (united)
 * @param multiple the divisor (numeric)
 * @return the quotient
 */
template <isQuantity Q> constexpr Q operator/(Q quantity, double divisor) { return Q(quantity.internal() / divisor); }

/**
 * @brief multiplies two unit quantities. The output type is determined using [Multiplied]
 *
 * @param lhs the multiplicand
 * @param rhs the multiplier
 * @return the product
 */
template <isQuantity Q1, isQuantity Q2, isQuantity Q3 = Multiplied<Q1, Q2>> Q3 constexpr operator*(Q1 lhs, Q2 rhs) {
    return Q3(lhs.internal() * rhs.internal());
}

/**
 * @brief divides two unit quantities. The output type is determined using <Divided>
 *
 * @param lhs the dividend
 * @param rhs the divisor
 * @return the quotient
 */
template <isQuantity Q1, isQuantity Q2, isQuantity Q3 = Divided<Q1, Q2>> Q3 constexpr operator/(Q1 lhs, Q2 rhs) {
    return Q3(lhs.internal() / rhs.internal());
}

/**
 * @brief checks if two isomorphic (equal dimensions) quantities have equal internal values
 * @param lhs the first quantity to compare
 * @param rhs the second quantity to compare
 * @return true if both quantities have the same value, false otherwise
 */
template <isQuantity Q, isQuantity R> constexpr bool operator==(const Q& lhs, const R& rhs)
    requires Isomorphic<Q, R>
{
    return (lhs.internal() == rhs.internal());
}

/**
 * @brief checks if two isomorphic (equal dimensions) quantities have differing internal values
 * @param lhs the first quantity to compare
 * @param rhs the second quantity to compare
 * @return true if both quantities have the same value, false otherwise
 */
template <isQuantity Q, isQuantity R> constexpr bool operator!=(const Q& lhs, const R& rhs)
    requires Isomorphic<Q, R>
{
    return (lhs.internal() != rhs.internal());
}

/**
 * @brief checks if a quantity has a lesser or equal value than another quantity with equal dimensions
 * @param lhs the first quantity to compare
 * @param rhs the second quantity to compare
 * @return true if the left hand quantity's value is less than or equal to the right hand quantity, false otherwise
 */
template <isQuantity Q, isQuantity R> constexpr bool operator<=(const Q& lhs, const R& rhs)
    requires Isomorphic<Q, R>
{
    return (lhs.internal() <= rhs.internal());
}

/**
 * @brief checks if a quantity has a greater or equal value than another quantity with equal dimensions
 * @param lhs the first quantity to compare
 * @param rhs the second quantity to compare
 * @return true if the left hand quantity's value is greater than or equal to the right hand quantity, false otherwise
 */
template <isQuantity Q, isQuantity R> constexpr bool operator>=(const Q& lhs, const R& rhs)
    requires Isomorphic<Q, R>
{
    return (lhs.internal() >= rhs.internal());
}

/**
 * @brief checks if a quantity has a lesser value than another quantity with equal dimensions
 * @param lhs the first quantity to compare
 * @param rhs the second quantity to compare
 * @return true if the left hand quantity's value is less than the right hand quantity, false otherwise
 */
template <isQuantity Q, isQuantity R> constexpr bool operator<(const Q& lhs, const R& rhs)
    requires Isomorphic<Q, R>
{
    return (lhs.internal() < rhs.internal());
}

/**
 * @brief checks if a quantity has a greater value than another quantity with equal dimensions
 * @param lhs the first quantity to compare
 * @param rhs the second quantity to compare
 * @return true if the left hand quantity's value is greater than the right hand quantity, false otherwise
 */
template <isQuantity Q, isQuantity R> constexpr bool operator>(const Q& lhs, const R& rhs)
    requires Isomorphic<Q, R>
{
    return (lhs.internal() > rhs.internal());
}

#define NEW_UNIT(Name, suffix, m, l, t, i, a, o, j, n)                                                                 \
    class Name : public Quantity<std::ratio<m>, std::ratio<l>, std::ratio<t>, std::ratio<i>, std::ratio<a>,            \
                                 std::ratio<o>, std::ratio<j>, std::ratio<n>> {                                        \
        public:                                                                                                        \
            /**                                                                                                        \
             @brief construct a new Name object<br>                                                                  \
             @param value the value of the new Name <br>                                                               \
             */                                                                                                        \
            explicit constexpr Name(double value)                                                                      \
                : Quantity<std::ratio<m>, std::ratio<l>, std::ratio<t>, std::ratio<i>, std::ratio<a>, std::ratio<o>,   \
                           std::ratio<j>, std::ratio<n>>(value) {}                                                     \
            /**                                                                                                        \
             @brief construct a new Name object<br>                                                                  \
             @param other the Name to copy<br>                                                                         \
             */                                                                                                        \
            constexpr Name(Quantity<std::ratio<m>, std::ratio<l>, std::ratio<t>, std::ratio<i>, std::ratio<a>,         \
                                    std::ratio<o>, std::ratio<j>, std::ratio<n>>                                       \
                               value)                                                                                  \
                : Quantity<std::ratio<m>, std::ratio<l>, std::ratio<t>, std::ratio<i>, std::ratio<a>, std::ratio<o>,   \
                           std::ratio<j>, std::ratio<n>>(value) {};                                                    \
    };                                                                                                                 \
    template <> struct LookupName<Quantity<std::ratio<m>, std::ratio<l>, std::ratio<t>, std::ratio<i>, std::ratio<a>,  \
                                           std::ratio<o>, std::ratio<j>, std::ratio<n>>> {                             \
            using Named = Name;                                                                                        \
    };                                                                                                                 \
    constexpr Name suffix = Name(1.0);                                                                                 \
    constexpr Name operator""_##suffix(long double value) {                                                            \
        return Name(Quantity<std::ratio<m>, std::ratio<l>, std::ratio<t>, std::ratio<i>, std::ratio<a>, std::ratio<o>, \
                             std::ratio<j>, std::ratio<n>>(static_cast<double>(value)));                               \
    }                                                                                                                  \
    constexpr Name operator""_##suffix(unsigned long long value) {                                                     \
        return Name(Quantity<std::ratio<m>, std::ratio<l>, std::ratio<t>, std::ratio<i>, std::ratio<a>, std::ratio<o>, \
                             std::ratio<j>, std::ratio<n>>(static_cast<double>(value)));                               \
    }                                                                                                                  \
    inline std::ostream& operator<<(std::ostream& os, const Name& quantity) {                                          \
        os << quantity.internal() << "_" << #suffix;                                                                   \
        return os;                                                                                                     \
    }                                                                                                                  \
    constexpr inline Name from_##suffix(double value) { return Name(value); }                                          \
    constexpr inline double to_##suffix(Name quantity) { return quantity.internal(); }

#define NEW_UNIT_LITERAL(Name, suffix, multiple)                                                                       \
    constexpr Name suffix = multiple;                                                                                  \
    constexpr Name operator""_##suffix(long double value) { return static_cast<double>(value) * multiple; }            \
    constexpr Name operator""_##suffix(unsigned long long value) { return static_cast<double>(value) * multiple; }     \
    constexpr inline Name from_##suffix(double value) { return value * multiple; }                                     \
    constexpr inline double to_##suffix(Name quantity) { return quantity.convert(multiple); }

#define NEW_METRIC_PREFIXES(Name, base)                                                                                \
    NEW_UNIT_LITERAL(Name, T##base, base * 1E12)                                                                       \
    NEW_UNIT_LITERAL(Name, G##base, base * 1E9)                                                                        \
    NEW_UNIT_LITERAL(Name, M##base, base * 1E6)                                                                        \
    NEW_UNIT_LITERAL(Name, k##base, base * 1E3)                                                                        \
    NEW_UNIT_LITERAL(Name, c##base, base / 1E2)                                                                        \
    NEW_UNIT_LITERAL(Name, m##base, base / 1E3)                                                                        \
    NEW_UNIT_LITERAL(Name, u##base, base / 1E6)                                                                        \
    NEW_UNIT_LITERAL(Name, n##base, base / 1E9)

NEW_UNIT(Number, num, 0, 0, 0, 0, 0, 0, 0, 0)
NEW_UNIT_LITERAL(Number, percent, num / 100.0);

NEW_UNIT(Mass, kg, 1, 0, 0, 0, 0, 0, 0, 0)
NEW_UNIT_LITERAL(Mass, g, kg / 1000)
NEW_UNIT_LITERAL(Mass, lb, g * 453.6)

NEW_UNIT(Time, sec, 0, 0, 1, 0, 0, 0, 0, 0)
NEW_METRIC_PREFIXES(Time, sec)
NEW_UNIT_LITERAL(Time, min, sec * 60)
NEW_UNIT_LITERAL(Time, hr, min * 60)
NEW_UNIT_LITERAL(Time, day, hr * 24)

NEW_UNIT(Length, m, 0, 1, 0, 0, 0, 0, 0, 0)
NEW_METRIC_PREFIXES(Length, m)
NEW_UNIT_LITERAL(Length, in, cm * 2.54)
NEW_UNIT_LITERAL(Length, ft, in * 12)
NEW_UNIT_LITERAL(Length, yd, ft * 3)
NEW_UNIT_LITERAL(Length, mi, ft * 5280)
NEW_UNIT_LITERAL(Length, tile, 600 * mm)

NEW_UNIT(Area, m2, 0, 2, 0, 0, 0, 0, 0, 0)
NEW_UNIT_LITERAL(Area, Tm2, Tm* Tm);
NEW_UNIT_LITERAL(Area, Gm2, Gm* Gm);
NEW_UNIT_LITERAL(Area, Mm2, Mm* Mm);
NEW_UNIT_LITERAL(Area, km2, km* km);
NEW_UNIT_LITERAL(Area, cm2, cm* cm);
NEW_UNIT_LITERAL(Area, mm2, mm* mm);
NEW_UNIT_LITERAL(Area, um2, um* um);
NEW_UNIT_LITERAL(Area, nm2, nm* nm);
NEW_UNIT_LITERAL(Area, in2, in* in)

NEW_UNIT(LinearVelocity, mps, 0, 1, -1, 0, 0, 0, 0, 0)
NEW_METRIC_PREFIXES(LinearVelocity, mps);
NEW_UNIT_LITERAL(LinearVelocity, mph, m / hr)
NEW_METRIC_PREFIXES(LinearVelocity, mph)
NEW_UNIT_LITERAL(LinearVelocity, inps, in / sec)
NEW_UNIT_LITERAL(LinearVelocity, miph, mi / hr)

NEW_UNIT(LinearAcceleration, mps2, 0, 1, -2, 0, 0, 0, 0, 0)
NEW_METRIC_PREFIXES(LinearAcceleration, mps2)
NEW_UNIT_LITERAL(LinearAcceleration, mph2, m / hr / hr)
NEW_METRIC_PREFIXES(LinearAcceleration, mph2)
NEW_UNIT_LITERAL(LinearAcceleration, inps2, in / sec / sec)
NEW_UNIT_LITERAL(LinearAcceleration, miph2, mi / hr / hr)

NEW_UNIT(LinearJerk, mps3, 0, 1, -3, 0, 0, 0, 0, 0)
NEW_METRIC_PREFIXES(LinearJerk, mps3)
NEW_UNIT_LITERAL(LinearJerk, mph3, m / (hr * hr * hr))
NEW_METRIC_PREFIXES(LinearJerk, mph3)
NEW_UNIT_LITERAL(LinearJerk, inps3, in / (sec * sec * sec))
NEW_UNIT_LITERAL(LinearJerk, miph3, mi / (hr * hr * hr))

NEW_UNIT(Curvature, radpm, 0, -1, 0, 0, 0, 0, 0, 0);

NEW_UNIT(Inertia, kgm2, 1, 2, 0, 0, 0, 0, 0, 0)

NEW_UNIT(Force, N, 1, 1, -2, 0, 0, 0, 0, 0)

NEW_UNIT(Torque, Nm, 1, 2, -2, 0, 0, 0, 0, 0)

NEW_UNIT(Power, watt, 1, 2, -3, 0, 0, 0, 0, 0)

NEW_UNIT(Current, amp, 0, 0, 0, 1, 0, 0, 0, 0)

NEW_UNIT(Charge, coulomb, 0, 0, 1, 1, 0, 0, 0, 0)

NEW_UNIT(Voltage, volt, 1, 2, -3, -1, 0, 0, 0, 0)
NEW_METRIC_PREFIXES(Voltage, volt);

NEW_UNIT(Resistance, ohm, 1, 2, -3, -2, 0, 0, 0, 0)
NEW_METRIC_PREFIXES(Resistance, ohm)

NEW_UNIT(Conductance, siemen, -1, -2, 3, 2, 0, 0, 0, 0)
NEW_METRIC_PREFIXES(Conductance, siemen);

NEW_UNIT(Luminosity, candela, 0, 0, 0, 0, 0, 0, 1, 0);

NEW_UNIT(Moles, mol, 0, 0, 0, 0, 0, 0, 0, 1);

namespace units {
/**
 * @brief takes the absolute value of a quantity
 *
 * @param lhs the quantity
 * @return constexpr Q the absolute value
 */
template <isQuantity Q> constexpr Q abs(const Q& lhs) { return Q(std::abs(lhs.internal())); }

/**
 * @brief takes the maximum of two isomorphic quantities
 *
 * @param lhs the first operand
 * @param rhs the second operand
 * @return constexpr Q
 */
template <isQuantity Q, isQuantity R> constexpr Q max(const Q& lhs, const R& rhs)
    requires Isomorphic<Q, R>
{
    return (lhs > rhs ? lhs : rhs);
}

/**
 * @brief takes the minimum of two isomorphic quantites (most conservative quantity)
 *
 * @param lhs the first operand
 * @param rhs the second operand
 * @return constexpr Q
 */
template <isQuantity Q, isQuantity R> constexpr Q min(const Q& lhs, const R& rhs)
    requires Isomorphic<Q, R>
{
    return (lhs < rhs ? lhs : rhs);
}

/**
 * @brief calculates the power of a quantity to a given integer
 *
 * @tparam R the power to raise the quantity to
 * @param lhs
 * @return constexpr S the result of the operation
 */
template <int R, isQuantity Q, isQuantity S = Exponentiated<Q, std::ratio<R>>> constexpr S pow(const Q& lhs) {
    return S(std::pow(lhs.internal(), R));
}

/**
 * @brief squares a quantity
 *
 * @param lhs the quantity
 * @return constexpr S the square of the quantity
 */
template <isQuantity Q, isQuantity S = Exponentiated<Q, std::ratio<2>>> constexpr S square(const Q& lhs) {
    return pow<2>(lhs);
}

/**
 * @brief takes the cube of a quantity
 *
 * @param lhs the quantity
 * @return constexpr S the cube of the quantity
 */
template <isQuantity Q, isQuantity S = Exponentiated<Q, std::ratio<3>>> constexpr S cube(const Q& lhs) {
    return pow<3>(lhs);
}

/**
 * @brief takes the R root of a quantity
 *
 * @tparam R the order of the root
 * @param lhs the quantity
 * @return constexpr S the R root of the quantity
 */
template <int R, isQuantity Q, isQuantity S = Rooted<Q, std::ratio<R>>> constexpr S root(const Q& lhs) {
    return S(std::pow(lhs.internal(), 1.0 / R));
}

/**
 * @brief takes the square root of a quantity
 *
 * @param lhs the quantity
 * @return constexpr S the square root of the quantity
 */
template <isQuantity Q, isQuantity S = Rooted<Q, std::ratio<2>>> constexpr S sqrt(const Q& lhs) { return root<2>(lhs); }

/**
 * @brief takes the cube root of a quantity
 *
 * @param lhs the quantity
 * @return constexpr S the cube root of the quantity
 */
template <isQuantity Q, isQuantity S = Rooted<Q, std::ratio<3>>> constexpr S cbrt(const Q& lhs) { return root<3>(lhs); }

/**
 * @brief calculates the hypotenuse of a right triangle with two sides of isomorphic quantities
 *
 * @param lhs x side
 * @param rhs y side
 * @return constexpr Q the hypotenuse
 */
template <isQuantity Q, isQuantity R> constexpr Q hypot(const Q& lhs, const R& rhs)
    requires Isomorphic<Q, R>
{
    return Q(std::hypot(lhs.internal(), rhs.internal()));
}

/**
 * @brief returns the remainder of a division of two isomorphic quantities
 *
 * @param lhs the dividend
 * @param rhs the divisor
 * @return constexpr Q the remainder
 */
template <isQuantity Q, isQuantity R> constexpr Q mod(const Q& lhs, const R& rhs)
    requires Isomorphic<Q, R>
{
    return Q(std::fmod(lhs.internal(), rhs.internal()));
}

/**
 * @brief returns the absolute value of x with the sign of y
 *
 * @param lhs the quantity to take the absolute value of (x)
 * @param rhs the quantity to take the sign of (y)
 * @return constexpr the first quantity with the sign of the second
 */
template <isQuantity Q1, isQuantity Q2> constexpr Q1 copysign(const Q1& lhs, const Q2& rhs) {
    return Q1(std::copysign(lhs.internal(), rhs.internal()));
}

/**
 * @brief returns the sign of a quantity
 *
 * @param lhs the quantity
 * @return constexpr int the sign of the quantity
 */
template <isQuantity Q> constexpr int sgn(const Q& lhs) { return lhs.internal() < 0 ? -1 : 1; }

/**
 * @brief returns true if the quantity is negative
 *
 * @param lhs the quantity
 * @return true if the quantity is negative, false otherwise
 */
template <isQuantity Q> constexpr bool signbit(const Q& lhs) { return std::signbit(lhs.internal()); }

/**
 * @brief clamps a quantity between two other isomporphic quantities
 *
 * @param lhs the quantity to clamp
 * @param lo the lower bound
 * @param hi the upper bound
 * @return constexpr Q the clamped quantity
 */
template <isQuantity Q, isQuantity R, isQuantity S> constexpr Q clamp(const Q& lhs, const R& lo, const S& hi)
    requires Isomorphic<Q, R, S>
{
    return Q(std::clamp(lhs.internal(), lo.internal(), hi.internal()));
}

template <isQuantity Q, isQuantity R> constexpr Q ceil(const Q& lhs, const R& rhs)
    requires Isomorphic<Q, R>
{
    return Q(std::ceil(lhs.internal() / rhs.internal()) * rhs.internal());
}

template <isQuantity Q, isQuantity R> constexpr Q floor(const Q& lhs, const R& rhs)
    requires Isomorphic<Q, R>
{
    return Q(std::floor(lhs.internal() / rhs.internal()) * rhs.internal());
}

template <isQuantity Q, isQuantity R> constexpr Q trunc(const Q& lhs, const R& rhs)
    requires Isomorphic<Q, R>
{
    return Q(std::trunc(lhs.internal() / rhs.internal()) * rhs.internal());
}

template <isQuantity Q, isQuantity R> constexpr Q round(const Q& lhs, const R& rhs)
    requires Isomorphic<Q, R>
{
    return Q(std::round(lhs.internal() / rhs.internal()) * rhs.internal());
}
} // namespace units

// Convert an angular unit `Q` to a linear unit correctly;
// mostly useful for velocities
template <isQuantity Q> Quantity<typename Q::mass, typename Q::angle, typename Q::time, typename Q::current,
                                 typename Q::length, typename Q::temperature, typename Q::luminosity, typename Q::moles>
toLinear(Quantity<typename Q::mass, typename Q::length, typename Q::time, typename Q::current, typename Q::angle,
                  typename Q::temperature, typename Q::luminosity, typename Q::moles>
             angular,
         Length diameter) {
    return unit_cast<Quantity<typename Q::mass, typename Q::angle, typename Q::time, typename Q::current,
                              typename Q::length, typename Q::temperature, typename Q::luminosity, typename Q::moles>>(
        angular * (diameter / 2.0));
}

// Convert an linear unit `Q` to a angular unit correctly;
// mostly useful for velocities
template <isQuantity Q> Quantity<typename Q::mass, typename Q::angle, typename Q::time, typename Q::current,
                                 typename Q::length, typename Q::temperature, typename Q::luminosity, typename Q::moles>
toAngular(Quantity<typename Q::mass, typename Q::length, typename Q::time, typename Q::current, typename Q::angle,
                   typename Q::temperature, typename Q::luminosity, typename Q::moles>
              linear,
          Length diameter) {
    return unit_cast<Quantity<typename Q::mass, typename Q::angle, typename Q::time, typename Q::current,
                              typename Q::length, typename Q::temperature, typename Q::luminosity, typename Q::moles>>(
        linear / (diameter / 2.0));
}
