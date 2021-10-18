#include "Headers.h"

using namespace rttr;

void toJsonRecursive(const instance& obj, json& writer);

/////////////////////////////////////////////////////////////////////////////////////////

bool WriteVariant(const variant& var, json& writer);

bool AttemptWriteFundementalType(const type& t, const variant& var, json& writer)
{
    // Json Number
    if (t.is_arithmetic())
    {
        if (t == type::get<bool>())
            writer = var.to_bool();
        else if (t == type::get<char>())
            writer = var.to_bool();
        else if (t == type::get<int>())
            writer = var.to_int();
        else if (t == type::get<uint64_t>())
            writer = var.to_uint64();
        else if (t == type::get<float>())
            writer = var.to_double();
        else if (t == type::get<double>())
            writer = var.to_double();
        return true;
    }
    // Enumeration as string
    else if (t.is_enumeration())
    {
        bool ok = false;
        // Attempt to serialize as string
        auto result = var.to_string(&ok);
        if (ok)
        {
            writer = var.to_string();
        }
        else
        {
            // Attempt to serialize as number
            auto value = var.to_uint64(&ok);
            if (ok)
                writer = uint64_t(value);
            else
                writer = nullptr;
        }

        return true;
    }
    // Strings!
    else if (t == type::get<std::string>())
    {
        writer = var.to_string();
        return true;
    }

    // Not a fundamental type we know how to process
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

static void WriteArray(const variant_sequential_view& view, json& writer)
{
    // Init array
    writer = json::array();
    int i = 0;
    for (const auto& item : view)
    {
        WriteVariant(item, writer[i]);
        i++;
    }
}


/////////////////////////////////////////////////////////////////////////////////////////

static void WriteAssociativeContainer(const variant_associative_view& view, json& writer)
{
    static const string_view key_name("key");
    static const string_view value_name("value");

    writer = json::array();
    int i = 0;
    // Dealing with keys = values containers like sets
    if (view.is_key_only_type())
    {
        for (auto& item : view)
        {
            WriteVariant(item.first, writer[i]);
            i++;
        }
    }
    else
    {
        for (auto& item : view)
        {
            WriteVariant(item.first, writer[i][key_name.data()]);
            WriteVariant(item.second, writer[i][value_name.data()]);
            i++;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

bool WriteVariant(const variant& var, json& writer)
{
    // Deal with wrapped type
    variant localVar = var;
    type varType = var.get_type();

    if(varType.is_wrapper())
    {
        varType = varType.get_wrapped_type();
        localVar = localVar.extract_wrapped_value();
    }

    if (AttemptWriteFundementalType(varType, localVar, writer))
    {
        // Successful write!
        return true;
    }
    // If its not a basic, is it a sequential?
    else if (var.is_sequential_container())
    {
        WriteArray(var.create_sequential_view(), writer);
    }
    else if (var.is_associative_container())
    {
        WriteAssociativeContainer(var.create_associative_view(), writer);
    }
    else
    {
        // Not a fundemental, or a container. Thus a OBJ.
        auto child_props = varType.get_properties();
        if (!child_props.empty())
        {
            toJsonRecursive(var, writer);
        }
        else
        {
            // Assert
            // Some unknown type that is not a fundamental, has no properties, and is not a data structure
            // Probably some registration issue
            // Or its a pointer! I handled pointers in here my game code
            assert("Unknown RTTR serilization edge case that we haven't discovered");
            return false;
        }
    }

    return true;
}


void toJsonRecursive(const instance& obj2, json& writer)
{
    instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;

    auto prop_list = obj.get_derived_type().get_properties();
    for (auto prop : prop_list)
    {
        variant prop_value = prop.get_value(obj);
        if (!prop_value)
            continue; // cannot serialize, because we cannot retrieve the value
        if (prop.get_metadata("NO_SERIALIZE"))
            continue;
        const auto name = prop.get_name();
        if (!WriteVariant(prop_value, writer[name.data()]))
        {
            std::cerr << "Failed to serialize" << name << std::endl;
        }
    }
}

// Kickstart function
json ToJson(rttr::instance obj, const char * name) {
    json jsonWriter;

    // Check instance for errors
    if (!obj.is_valid())
        return std::string();

    toJsonRecursive(obj, jsonWriter[name]);

    return jsonWriter;
}
