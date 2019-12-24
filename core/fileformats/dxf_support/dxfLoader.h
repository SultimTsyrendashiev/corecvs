//
// Created by Myasnikov Vladislav on 17.10.2019.
//

#ifndef DXF_SUPPORT_DXFLOADER_H
#define DXF_SUPPORT_DXFLOADER_H

#include "core/fileformats/dxf_support/dxfCodes.h"
#include "core/fileformats/dxf_support/dxfBuilder.h"
#include "core/buffers/rgb24/wuRasterizer.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/fileformats/bufferLoader.h"
#include "core/math/vector/vector2d.h"
#include "core/math/vector/vector3d.h"
#include <map>

namespace corecvs {

class DxfLoader {
public:
    explicit DxfLoader(DxfBuilder &dxfBuilder)
    : dxfBuilder(dxfBuilder) {}

    int load(std::string const &fileName);

private:
    DxfBuilder &dxfBuilder;
    DxfElementType currentEntityType = DxfElementType::DXF_UNKNOWN_TYPE;
    DxfElementType currentElementType = DxfElementType::DXF_UNKNOWN_TYPE;
    std::string variableName;
    std::map<int, std::string> rawValues;
    std::vector<Vector2d<double>> current2dVertices = {};     // vertices of LwPolylineEntity
    std::vector<DxfVertexData*> current3dVertices = {};       // vertices of PolylineEntity
    DxfPolylineData *polylineData = nullptr;
    DxfBlock *currentBlock = nullptr;

    int processDxfPair(int code, std::string const &value);
    void addVariable();
    void addLayer();
    void addLineType();
    void addBlockRecord();
    void addLine();
    void addLwPolyline();
    void addPolyline();
    void addCircle();
    void addCircularArc();
    void addEllipticalArc();
    void addPoint();
    void addBlock();
    void addBlockReference();
    void handleLwPolyline(int groupCode);
    void handlePolyline();
    void handleVertex();
    void handleVertexSequence();
    void handleBlock();
    DxfEntityData getEntityData();
    DxfObjectData getObjectData();
    static bool getTruncatedLine(std::string &s, std::istream &stream);

    bool hasRawValue(int code) {
        return rawValues.count(code) == 1;
    }

    int getIntValue(int code, int defValue) {
        if (!hasRawValue(code)) {
            return defValue;
        }
        char* p;
        return (int) std::strtol(rawValues[code].c_str(), &p, 10);
    }

    double getDoubleValue(int code, double defValue) {
        if (!hasRawValue(code)) {
            return defValue;
        }
        char* p;
        return std::strtod(rawValues[code].c_str(), &p);
    }

    std::string getStringValue(int code, std::string const &defValue) {
        if (!hasRawValue(code)) {
            return defValue;
        }
        return rawValues[code];
    }
};

class DXFToRGB24BufferLoader : public BufferLoader<RGB24Buffer> {
public:
    DXFToRGB24BufferLoader() = default;
    ~DXFToRGB24BufferLoader() override = default;

    bool acceptsFile(std::string const &name) override;
    RGB24Buffer* load(std::string const &name) override;

    std::string name() override { return "DXFToRGB24BufferLoader"; }
    std::vector<std::string> extentions() override { return {extension}; }
    static string extension;
};

} // namespace corecvs

#endif //DXF_SUPPORT_DXFLOADER_H
