#pragma once

#include <bitset>
#include <unordered_map>
#include "stringutils.h"

// Helpers/utilities for getting/matching instruction attributes.
namespace ShaderInstruction
{
    // All usages of UnderlyingEnumType should be changed to std::underlying_type when this test is moved to STL_VER=110.
    typedef unsigned UnderlyingEnumType;

    // These macros make it so that there is only one place where an enum value needs to be added and corresponding changes
    // required are automatically generated when said value is added. All the ForEach* macro take a single parameters which
    // should be another macro which takes 3 parameters. The passed in macro will be called once for each value in an enumeration.
    // Parameters passed to passed in macro: name of enum value, corresponding assembly string (if applicable) and a numeric value
    // which specifies by which amount 1 should be left shifted for a certain value.

    // To add a new enum, add a new entry (call it name) to ForEachEnumType and then define a new ForEachName(f) the same way as Opcode/Modifier.
    // Adding a new entry to an enumeration (Opcode etc.) is as trivial as adding a new entry into the corresponding ForEach*.
    #define ForEachEnumType(f) \
        f(Opcode,,0) \
        f(Modifier,,1)

    #define ForEachOpcode(f) \
        f(Gather4,         "gather4", 0) \
        f(Sample,          "sample",  1) \
        f(Load,            "ld",      2) \
        f(LOD,             "lod",     3) \
        f(Load2dMultiSamp, "ld2dms",  4) \
        f(ResourceInfo,    "resinfo", 5) \
        f(BufferInfo,      "bufinfo", 6)

    #define ForEachModifier(f) \
        f(ResidencyFeedback,  "s",          0) \
        f(Clamp,              "cl",         1) \
        f(ProgrammableOffset, "po",         2) \
        f(Comparison,         "c",          3) \
        f(ScalarLOD,          "l",          4) \
        f(LevelZero,          "lz",         5) \
        f(Derivative,         "d",          6) \
        f(Raw,                "raw",        7) \
        f(Structured,         "structured", 8) \
        f(LODBias,            "b",          9)

    // This macro when passed to a ForEach* will generate all the actual enumeration values for that EnumType. Second param
    // not used.
    #define AddEnumEntry(name, _0, val) name = 1 << val##,

    enum class EnumType : UnderlyingEnumType
    {
        ForEachEnumType(AddEnumEntry)
    };

    enum class Opcode : UnderlyingEnumType
    {
        None = 0,
        ForEachOpcode(AddEnumEntry)
    };

    enum class Modifier : UnderlyingEnumType
    {
        None = 0,
        ForEachModifier(AddEnumEntry)
    };

    #undef AddEnumEntry

    // Trivial helper for RecordSize macro. All parameters ignored.
    #define Incr(_0, _1, _2) + 1

    // Macro to figure out # of elements in corresponding enumerations and also define the lower + upper bound for each enum type.
    // For example the Opcode enum will have these variables defined after a call to this macro:
    //     OpcodeCount, OpcodeUpperBound & OpcodeLowerBound.
    #define RecordSize(name) const unsigned name##Count = 0 ForEach##name##(Incr); \
                             const name name##UpperBound = static_cast<##name##>(1 << name##Count); \
                             const name name##LowerBound = static_cast<##name##>(1);

    RecordSize(Opcode);
    RecordSize(Modifier);

    #undef RecordSize
    #undef Incr

    using std::unordered_map;
    using std::bitset;

    class LookupHelper;

    // Stores all attributes for a given instruction. Instruction string must be provided to constructor. After object has been constructed, all
    // attributes are immutable.
    class Attributes
    {
        public:
            Attributes(const tstring&);
            Attributes(const TCHAR*);
            bool ModifiersMatch(const Modifier) const;
            bool OpcodeMatch(const Opcode) const;
            bool FullMatch(const Opcode, const Modifier, const Modifier) const;
            const tstring GetInstruction() const;

        private:
            Attributes();
            Attributes(const Attributes&);
            Attributes& operator=(const Attributes&);

            void Init();
            void HandleOpcode(const tstring&);
            void HandleModifier(const tstring&);
            bool ModifiersMatch(const Modifier, const Modifier) const;

            const tstring InstStr;
            bitset<ModifierCount> ModifierBitmap;
            Opcode Op;
    };

    // Constant static data.
    class LookupHelper
    {
        public:
            static const size_t Index(const UnderlyingEnumType);
            static const Opcode Opcode(const tstring&);
            static const Modifier Modifier(const tstring&);

        private:
            template<typename T, typename U> static const U Get(const unordered_map<T, U>&, const T&);
            static const unordered_map<UnderlyingEnumType, size_t> IndexMap;
            static const decltype(LookupHelper::IndexMap) MakeLogTable();

            static const unordered_map<tstring, ::ShaderInstruction::Opcode> OpcodeStringMap;
            static const unordered_map<tstring, ::ShaderInstruction::Modifier> ModifierStringMap;
            static const decltype(LookupHelper::OpcodeStringMap) MakeOpcodeStringMap();
            static const decltype(LookupHelper::ModifierStringMap) MakeModifierStringMap();
    };

    typedef LookupHelper Get;

    namespace Operators
    {
        // Macro which will declare all operator overloads for each EnumType. Second & third parameters not used.
        #define OperatorsDecl(name, _0, _1) \
        const name operator|(const name##, const name##); \
        const name operator&(const name##, const name##); \
        const name operator~(const name##); \
        const name operator<<(const name##, const unsigned); \
        const name##& operator++(##name##&); \
        const name operator++(##name##&, int); \
        const bool operator&&(const name##, const bool); \
        const bool operator!(const name##);

        ForEachEnumType(OperatorsDecl);

        #undef OperatorsDecl
    }
}

using ShaderInstruction::Opcode;
using ShaderInstruction::Modifier;
using ShaderInstruction::Attributes;
using namespace ShaderInstruction::Operators;