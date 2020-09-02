#include <algine/core/texture/TextureManager.h>

#include <algine/core/texture/Texture.h>
#include <algine/core/JsonHelper.h>

using namespace std;
using namespace nlohmann;

#define constant(name, val) constexpr char name[] = val

namespace Config {
constant(Type, "type");
constant(Texture2D, "texture2d");
constant(TextureCube, "textureCube");

constant(Width, "width");
constant(Height, "height");

constant(Format, "format");

// base formats
constant(DepthComponent, "depthComponent");
constant(DepthStencil, "depthStencil");

constant(Red, "red");
constant(RG, "rg");
constant(RGB, "rgb");
constant(RGBA, "rgba");

// some sized formats
constant(Red16F, "red16f");
constant(RG16F, "rg16f");
constant(RGB16F, "rgb16f");
constant(RGBA16F, "rgba16f");

constant(Red32F, "red32f");
constant(RG32F, "rg32f");
constant(RGB32F, "rgb32f");
constant(RGBA32F, "rgba32f");

constant(Params, "params");

// param keys
constant(MinFilter, "minFilter");
constant(MagFilter, "magFilter");
constant(WrapU, "wrapU");
constant(WrapV, "wrapV");
constant(WrapW, "wrapW");

// param values
constant(Nearest, "nearest");
constant(Linear, "linear");
constant(ClampToEdge, "clampToEdge");
constant(ClampToBorder, "clampToBorder");
constant(Repeat, "repeat");
constant(MirroredRepeat, "mirroredRepeat");
constant(MirrorClampToEdge, "mirrorClampToEdge");

constant(File, "file");
constant(DataType, "dataType");

// data types
constant(Byte, "byte");
constant(UnsignedByte, "ubyte");
constant(Short, "short");
constant(UnsignedShort, "ushort");
constant(Int, "int");
constant(UnsignedInt, "uint");
constant(Float, "float");
constant(HalfFloat, "halfFloat");
constant(Double, "double");
}

namespace algine {
TextureManager::TextureManager()
    : m_type(),
      m_dataType(DataType::UnsignedByte),
      m_format(Texture::RGB),
      m_width(), // width & height will be overwritten
      m_height(), // if texture loaded from file
      m_writeFileSection()
{
    // m_defaultParams initialized in derived class
}

void TextureManager::setType(Type type) {
    m_type = type;
}

void TextureManager::setDataType(DataType dataType) {
    m_dataType = dataType;
}

void TextureManager::setFormat(uint format) {
    m_format = format;
}

void TextureManager::setWidth(uint width) {
    m_width = width;
}

void TextureManager::setHeight(uint height) {
    m_height = height;
}

void TextureManager::setParams(const map<uint, uint> &params) {
    m_params = params;
}

void TextureManager::setDefaultParams(const map<uint, uint> &defaultParams) {
    m_defaultParams = defaultParams;
}

TextureManager::Type TextureManager::getType() const {
    return m_type;
}

DataType TextureManager::getDataType() const {
    return m_dataType;
}

uint TextureManager::getFormat() const {
    return m_format;
}

uint TextureManager::getWidth() const {
    return m_width;
}

uint TextureManager::getHeight() const {
    return m_height;
}

const map<uint, uint>& TextureManager::getParams() const {
    return m_params;
}

const map<uint, uint>& TextureManager::getDefaultParams() const {
    return m_defaultParams;
}

#define format_uint(format) if (str == Config::format) return Texture::format;

inline uint stringToFormat(const string &str) {
    format_uint(DepthComponent)
    format_uint(DepthStencil)

    format_uint(Red)
    format_uint(RG)
    format_uint(RGB)
    format_uint(RGBA)

    format_uint(Red16F)
    format_uint(RG16F)
    format_uint(RGB16F)
    format_uint(RGBA16F)

    format_uint(Red32F)
    format_uint(RG32F)
    format_uint(RGB32F)
    format_uint(RGBA32F)

    throw runtime_error("Unsupported format '" + str + "'");
}

#define format_str(format) if (formatValue == Texture::format) return Config::format;

inline string formatToString(uint formatValue) {
    format_str(DepthComponent)
    format_str(DepthStencil)

    format_str(Red)
    format_str(RG)
    format_str(RGB)
    format_str(RGBA)

    format_str(Red16F)
    format_str(RG16F)
    format_str(RGB16F)
    format_str(RGBA16F)

    format_str(Red32F)
    format_str(RG32F)
    format_str(RGB32F)
    format_str(RGBA32F)

    throw runtime_error("Unsupported format " + to_string(formatValue));
}

#define param_uint(element) if (str == Config::element) return Texture::element;

inline uint stringToParamKey(const string &str) {
    param_uint(MinFilter)
    param_uint(MagFilter)
    param_uint(WrapU)
    param_uint(WrapV)
    param_uint(WrapW)

    throw runtime_error("Unsupported param key '" + str + "'");
}

inline uint stringToParamValue(const string &str) {
    param_uint(Nearest)
    param_uint(Linear)
    param_uint(ClampToEdge)
    param_uint(ClampToBorder)
    param_uint(Repeat)
    param_uint(MirroredRepeat)
    param_uint(MirrorClampToEdge)

    throw runtime_error("Unsupported param value '" + str + "'");
}

#define param_str(element) if (param == Texture::element) return Config::element;

inline string paramKeyToString(uint param) {
    param_str(MinFilter)
    param_str(MagFilter)
    param_str(WrapU)
    param_str(WrapV)
    param_str(WrapW)

    throw runtime_error("Unsupported param key " + to_string(param));
}

inline string paramValueToString(uint param) {
    param_str(Nearest)
    param_str(Linear)
    param_str(ClampToEdge)
    param_str(ClampToBorder)
    param_str(Repeat)
    param_str(MirroredRepeat)
    param_str(MirrorClampToEdge)

    throw runtime_error("Unsupported param value " + to_string(param));
}

#define dataType_type(element) if (str == Config::element) return DataType::element;

inline DataType stringToDataType(const string &str) {
    dataType_type(Byte)
    dataType_type(UnsignedByte)
    dataType_type(Short)
    dataType_type(UnsignedShort)
    dataType_type(Int)
    dataType_type(UnsignedInt)
    dataType_type(Float)
    dataType_type(HalfFloat)
    dataType_type(Double)

    throw runtime_error("Unsupported DataType '" + str + "'");
}

#define dataType_str(element) if (dataType == DataType::element) return Config::element;

inline string dataTypeToString(DataType dataType) {
    dataType_str(Byte)
    dataType_str(UnsignedByte)
    dataType_str(Short)
    dataType_str(UnsignedShort)
    dataType_str(Int)
    dataType_str(UnsignedInt)
    dataType_str(Float)
    dataType_str(HalfFloat)
    dataType_str(Double)

    throw runtime_error("Unsupported DataType " + to_string(static_cast<uint>(dataType)));
}

void TextureManager::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &config = jsonHelper.json;

    // load type
    m_type = map<string, Type> {
        {Texture2D, Type::Texture2D},
        {TextureCube, Type::TextureCube}
    } [config[Config::Type]];

    // load data type
    if (config.contains(File) && config[File].contains(Config::DataType))
        m_dataType = stringToDataType(config[File][Config::DataType]);

    // load width & height
    m_width = jsonHelper.readValue<int>(Width);
    m_height = jsonHelper.readValue<int>(Height);

    // load format
    if (config.contains(Format))
        m_format = stringToFormat(config[Format]);

    // load params
    if (config.contains(Params)) {
        for (const auto & p : config[Params].items()) {
            m_params[stringToParamKey(p.key())] = stringToParamValue(p.value());
        }
    } else {
        m_params = m_defaultParams;
    }

    ManagerBase::import(jsonHelper);
}

JsonHelper TextureManager::dump() {
    using namespace Config;

    json config;

    // write type
    config[Config::Type] = vector<string> {Texture2D, TextureCube} [static_cast<uint>(m_type)];

    // write data type
    if (m_writeFileSection)
        config[File][Config::DataType] = dataTypeToString(m_dataType);

    // write width & height if not zero
    if (m_width != 0)
        config[Width] = m_width;

    if (m_height != 0)
        config[Height] = m_height;

    // write format
    config[Format] = formatToString(m_format);

    // write params
    for (const auto & param : m_params)
        config[Params][paramKeyToString(param.first)] = paramValueToString(param.second);

    JsonHelper result(config);
    result.append(ManagerBase::dump());

    return result;
}
}
