#include "ShaderInstruction.h"
#include <sstream>
#include <assert.h>

// Attributes implementation + helpers used by Attributes.
namespace ShaderInstruction
{
    template<typename T> const UnderlyingEnumType AsRaw(const T t)
    {
        return static_cast<UnderlyingEnumType>(t);
    }

    template<typename T> const size_t AsIndex(const T t)
    {
        return Get::Index(AsRaw(t));
    }

    Attributes::Attributes(const tstring& Instruction) : InstStr(Instruction)
    {
        Init();
    }

    Attributes::Attributes(const TCHAR* Instruction) : InstStr(Instruction)
    {
        Init();
    }

    bool Attributes::ModifiersMatch(const Modifier set) const
    {
        return ModifiersMatch(set, Modifier::None);
    }
    
    bool Attributes::OpcodeMatch(const Opcode o) const
    {
        for (Opcode mask = OpcodeLowerBound; mask != OpcodeUpperBound; ++mask)
        {
            if ((mask & o) && (mask == Op))
            {
                return true;
            }
        }
        return false;
    }

    bool Attributes::FullMatch(const Opcode o, const Modifier require, const Modifier allow) const
    {
        assert(o != Opcode::None); // Use modifier specific queries if opcode need not be matched.
        assert(!(require & allow)); // Any common modifiers will be considered required.
        return OpcodeMatch(o) && ModifiersMatch(require, ~(require | allow));
    }

    bool Attributes::ModifiersMatch(const Modifier set, const Modifier unset) const
    {
        assert(!(set & unset)); // Logical error.
        for (Modifier mask = ModifierLowerBound; mask != ModifierUpperBound; ++mask)
        {
            const bool b = ModifierBitmap[AsIndex(mask)];
            if (((mask & set) && !b) || ((mask & unset) && b))
            {
                return false;
            }
        }
        return true;
    }

    using std::getline;

    void Attributes::Init()
    {
        const TCHAR delimiter = _T('_');
        std::basic_stringstream<TCHAR> s(InstStr);

        // Handle opcode explicitly
        {
            tstring opcode;
            getline(s, opcode, delimiter);
            HandleOpcode(opcode);
        }

        // Everything left has to be a modifier
        tstring current;
        while (getline(s, current, delimiter))
        {
            HandleModifier(current);
        }
    }

    void Attributes::HandleOpcode(const tstring& s)
    {
        Op = Get::Opcode(s);
    }
    
    void Attributes::HandleModifier(const tstring& s)
    {
        const Modifier m = Get::Modifier(s);
        const size_t index = AsIndex(m);
        ModifierBitmap.set(index);
    }

    const tstring Attributes::GetInstruction() const
    {
        return InstStr;
    }
}

// Helpers for accessing constant data
namespace ShaderInstruction
{   
    template<typename T, typename U> const U LookupHelper::Get(const unordered_map<T, U>& map, const T& key)
    {
        auto found =  map.find(key);
        assert(found != map.end());
        return found->second;
    }

    const size_t LookupHelper::Index(const UnderlyingEnumType i)
    {
        return Get(IndexMap, i);
    }

    const Opcode LookupHelper::Opcode(const tstring& s)
    {
        return Get(OpcodeStringMap, s);
    }

    const Modifier LookupHelper::Modifier(const tstring& s)
    {
        return Get(ModifierStringMap, s);
    }
}

// Operators
namespace ShaderInstruction
{
    namespace OperatorDefinition
    {
        template<typename T> T operator|(const T a, const T b)
        {
            return static_cast<T>(AsRaw(a) | AsRaw(b));
        }
        template<typename T> T operator&(const T a, const T b)
        {
            return static_cast<T>(AsRaw(a) & AsRaw(b));
        }
        template<typename T> T operator~(const T a)
        {
            return static_cast<T>(~AsRaw(a));
        }
        template<typename T> T operator<<(const T a, const unsigned shift)
        {
            return static_cast<T>(AsRaw(a) << shift);
        }
        template<typename T> T& operator++(T& a)
        {
            a = a << 1;
            return a;
        }
        template<typename T> T operator++(T& a, int)
        {
            T t = a;
            ++a;
            return t;
        }
        template<typename T> bool operator&&(const T a, const bool b)
        {
            return AsRaw(a) && b;
        }
        template<typename T> bool operator!(const T a)
        {
            return !(AsRaw(a));
        }
    }

    namespace Operators
    {
        // This macro will provide the definiton for all operator overloads for each EnumType. These will forward all operators
        // to the appropriate templatized operator in namespace OperatorDefinition. 2nd and 3rd paramaters ignored.
        #define Operators(name, _0, _1) \
        using ShaderInstruction::##name##; \
        const name operator|(const name a, const name b)          { return OperatorDefinition::operator|<##name##>(a, b); } \
        const name operator&(const name a, const name b)          { return OperatorDefinition::operator&<##name##>(a, b); } \
        const name operator~(const name a)                        { return OperatorDefinition::operator~<##name##>(a); } \
        const name operator<<(const name a, const unsigned b)     { return OperatorDefinition::operator<<<##name##>(a, b); } \
        const name##& operator++(name##& a)                       { return OperatorDefinition::operator++<##name##>(a); } \
        const name operator++(name##& a, int)                     { return OperatorDefinition::operator++<##name##>(a, 0); } \
        const bool operator&&(const name a, const bool b)         { return OperatorDefinition::operator&&<##name##>(a, b); } \
        const bool operator!(const name a)                        { return OperatorDefinition::operator!<##name##>(a); }

        ForEachEnumType(Operators);

        #undef Operators
    }
}

#include <type_traits>

// Constant static data.
namespace ShaderInstruction
{
    using std::remove_const;
    using std::add_const;

    add_const<decltype(LookupHelper::OpcodeStringMap)>::type LookupHelper::OpcodeStringMap = LookupHelper::MakeOpcodeStringMap();
    add_const<decltype(LookupHelper::ModifierStringMap)>::type LookupHelper::ModifierStringMap = LookupHelper::MakeModifierStringMap();
    add_const<decltype(LookupHelper::IndexMap)>::type LookupHelper::IndexMap = LookupHelper::MakeLogTable();

    // This macro will insert all mappings from the assembly string of an opcode to the appropriate value in the Opcode enum.
    #define Entry(name, asm, _) map[tstring(_T(asm))] = ShaderInstruction::Opcode::##name##;

    decltype(LookupHelper::OpcodeStringMap) LookupHelper::MakeOpcodeStringMap()
    {
        remove_const<decltype(LookupHelper::OpcodeStringMap)>::type map;
        ForEachOpcode(Entry);
        return map;
    }

    #undef Entry

    // This macro will insert all mappings from the assembly string of a modifier to the appropriate value in the Modifier enum.
    #define Entry(name, asm, _) map[tstring(_T(asm))] = ShaderInstruction::Modifier::##name##;

    decltype(LookupHelper::ModifierStringMap) LookupHelper::MakeModifierStringMap()
    {
        remove_const<decltype(LookupHelper::ModifierStringMap)>::type map;
        ForEachModifier(Entry);
        return map;
    }

    #undef Entry

    decltype(LookupHelper::IndexMap) LookupHelper::MakeLogTable()
    {
        remove_const<decltype(LookupHelper::IndexMap)>::type map;
        size_t log = 0;
        for (UnderlyingEnumType i = 1; i != 0; i <<= 1, ++log)
        {
            map[i] = log;
        }
        return map;
    }
}