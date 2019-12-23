//
// Created by Myasnikov Vladislav on 10/27/19.
//
#include <iostream>

#include "core/fileformats/dxf_support/entities/dxfEntity.h"
#include "core/geometry/conic.h"
#include "core/utils/utils.h"
#include "core/buffers/rgb24/bezierRasterizer.h"
#include "core/buffers/rgb24/wuRasterizer.h"
#include "core/buffers/rgb24/abstractPainter.h"

namespace corecvs {

// Data printing
void DxfEntity::print() {
    std::cout << "Handle: " << data.handle << std::endl;
    std::cout << "Layer name: " << data.layerName << std::endl;
    std::cout << "Line type name: " << data.lineTypeName << std::endl;
    std::cout << "RGB color: " << (int) data.rgbColor.r() << " " << (int) data.rgbColor.g() << " " << (int) data.rgbColor.b() << " " << std::endl;
    std::cout << "Color number: " << data.colorNumber << std::endl;
    std::cout << "Visibility: " << (data.isVisible ? "on" : "off") << std::endl;
}

void DxfLineEntity::print() {
    std::cout << "* * * Line Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Start point: " << data.startPoint << std::endl;
    std::cout << "End point: " << data.endPoint << std::endl;
    std::cout << std::endl;
}

void DxfLwPolylineEntity::print() {
    std::cout << "* * * LwPolyline Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Thickness: " << data.thickness << std::endl;
    std::cout << "Vertex amount: " << data.vertices.size() << std::endl;
    int i = 1;
    for (Vector2d vertex : data.vertices) std::cout << "Vertex " << i++ << ": " << vertex << std::endl;
    std::cout << std::endl;
}

void DxfPolylineEntity::print() {
    std::cout << "* * * Polyline Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Thickness: " << data.thickness << std::endl;
    std::cout << "Vertex amount: " << data.vertices.size() << std::endl;
    int i = 1;
    for (DxfVertexData* vertex : data.vertices) {
        std::cout << "Vertex " << i++ << ": " << vertex->location << std::endl;
        std::cout << "+-->Bulge: " << vertex->bulge << std::endl;
    }
    std::cout << std::endl;
}

void DxfCircleEntity::print() {
    std::cout << "* * * Circle Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Center point: " << data.center << std::endl;
    std::cout << "Radius: " << data.radius << std::endl;
    std::cout << "Thickness: " << data.thickness << std::endl;
    std::cout << std::endl;
}

void DxfCircularArcEntity::print() {
    std::cout << "* * * Circular Arc Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Center point: " << data.center << std::endl;
    std::cout << "Radius: " << data.radius << std::endl;
    std::cout << "Thickness: " << data.thickness << std::endl;
    std::cout << "Angle range: " << data.startAngle << ".." << data.endAngle << std::endl;
    std::cout << std::endl;
}

void DxfEllipticalArcEntity::print() {
    std::cout << "* * * Elliptical Arc Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Center point: " << data.center << std::endl;
    std::cout << "End point of major axis: " << data.majorAxisEndPoint << std::endl;
    std::cout << "Ratio: " << data.ratio << std::endl;
    std::cout << "Angle range: " << data.startAngle << ".." << data.endAngle << std::endl;
    std::cout << std::endl;
}

void DxfPointEntity::print() {
    std::cout << "* * * Point Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Location: " << data.location << std::endl;
    std::cout << "Thickness: " << data.thickness << std::endl;
    std::cout << std::endl;
}

// Drawing
void DxfLineEntity::draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) {
    auto startPoint = attrs->getDrawingValues(data.startPoint.x(), data.startPoint.y());
    auto endPoint = attrs->getDrawingValues(data.endPoint.x(), data.endPoint.y());
    buffer->drawLine(startPoint, endPoint, data.rgbColor);
}

void DxfLwPolylineEntity::draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) {
    int vertexNumber = data.vertices.size();
    if (vertexNumber > 1) {
        for (unsigned long i = 0; i < vertexNumber - !data.isClosed; i++) {
            auto startPoint = attrs->getDrawingValues(data.vertices[i % vertexNumber].x(), data.vertices[i % vertexNumber].y());
            auto endPoint = attrs->getDrawingValues(data.vertices[(i+1) % vertexNumber].x(), data.vertices[(i+1) % vertexNumber].y());
            buffer->drawLine(startPoint, endPoint, data.rgbColor);
        }
    } else if (vertexNumber == 1) {
        auto point = attrs->getDrawingValues(data.vertices[0].x(), data.vertices[0].y());
        buffer->drawPixel(point, data.rgbColor);
    }
}

void DxfPolylineEntity::draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) {
    int vertexNumber = data.vertices.size();
    if (vertexNumber > 1) {
        for (unsigned long i = 0; i < vertexNumber - !data.isClosed; i++) {
            Vector2dd startPoint = data.vertices[i     % vertexNumber]->location.xy();
            Vector2dd endPoint   = data.vertices[(i+1) % vertexNumber]->location.xy();
            auto bulge = data.vertices[i % vertexNumber]->bulge;

            if (bulge == 0) {
                startPoint = attrs->getDrawingValues(startPoint);
                endPoint = attrs->getDrawingValues(endPoint);
                buffer->drawLine(startPoint, endPoint, data.rgbColor);
            } else {
                auto delta = startPoint - endPoint;
                auto chordLength = delta.l2Metric();
                auto radius = chordLength / 4 * (1 / std::abs(bulge) + std::abs(bulge));
                auto mediumPoint = (startPoint + endPoint) / 2;
                auto apothemLength = std::sqrt(radius * radius - (chordLength / 2) * (chordLength / 2));
                auto startEndPointAngle = std::atan(delta.y() / delta.x());
                if (delta.x() < 0) startEndPointAngle += M_PI;
                auto centerPoint = mediumPoint + apothemLength * (
                        bulge < 0
                        ? Vector2dd(std::cos(startEndPointAngle + M_PI / 2), std::sin(startEndPointAngle + M_PI / 2))
                        : Vector2dd(std::cos(startEndPointAngle - M_PI / 2), std::sin(startEndPointAngle - M_PI / 2))
                        );

                auto startAngle = std::atan((startPoint.y() - centerPoint.y()) / (startPoint.x() - centerPoint.x()));
                if (startPoint.x() - centerPoint.x() < 0) startAngle += M_PI;
                auto endAngle = std::atan((endPoint.y() - centerPoint.y()) / (endPoint.x() - centerPoint.x()));
                if (endPoint.x() - centerPoint.x() < 0) endAngle += M_PI;

                auto circularArcData = new DxfCircularArcData(data, Vector3dd(centerPoint.x(), centerPoint.y(), 0), radius, data.thickness, radToDeg(startAngle), radToDeg(endAngle));
                auto circularArc = new DxfCircularArcEntity(*circularArcData);

                auto wasClockwiseDirection = attrs->isClockwiseDirection();
                attrs->setClockwiseDirection(bulge < 0);
                circularArc->draw(buffer, attrs);
                attrs->setClockwiseDirection(wasClockwiseDirection);
            }
        }
    } else if (vertexNumber == 1) {
        auto point = attrs->getDrawingValues(data.vertices[0]->location.x(), data.vertices[0]->location.y());
        buffer->drawPixel(point, data.rgbColor);
    }
}

void DxfCircleEntity::draw(class corecvs::RGB24Buffer *buffer, class corecvs::DxfDrawingAttrs *attrs) {
    auto centerPoint = attrs->getDrawingValues(data.center.x(), data.center.y());
    Circle2d circle(centerPoint, attrs->getDrawingValue(data.radius));
    buffer->drawArc(circle, data.rgbColor);
}

void DxfCircularArcEntity::draw(class corecvs::RGB24Buffer *buffer, class corecvs::DxfDrawingAttrs *attrs) {
    auto centerPoint = Vector2dd(data.center.x(), data.center.y());
    auto startAngle = degToRad(data.startAngle);
    auto endAngle = degToRad(data.endAngle);

    // divide arc into 1 or 2 segments
    std::vector<Vector2dd> controlAngles;
    if (attrs->isClockwiseDirection()) {
        while (endAngle > startAngle) endAngle -= 2 * M_PI;
        controlAngles.emplace_back(Vector2dd(startAngle, std::max(endAngle, startAngle - M_PI + 0.00001)));
        if (startAngle - endAngle > M_PI) controlAngles.emplace_back(Vector2dd(startAngle - M_PI, endAngle));
    } else {
        while (endAngle < startAngle) endAngle += 2 * M_PI;
        controlAngles.emplace_back(Vector2dd(startAngle, std::min(endAngle, startAngle + M_PI - 0.00001)));
        if (endAngle - startAngle > M_PI) controlAngles.emplace_back(Vector2dd(startAngle + M_PI, endAngle));
    }

    // draw 1 or 2 segments
    for (Vector2dd alpha : controlAngles) {
        auto a = Vector2dd(std::cos(alpha.x()), std::sin(alpha.x())) * data.radius;
        auto b = Vector2dd(std::cos(alpha.y()), std::sin(alpha.y())) * data.radius;
        auto r = std::sqrt(a.x() * a.x() + a.y() * a.y());
        auto d = std::sqrt((a.x() + b.x()) * (a.x() + b.x()) + (a.y() + b.y()) * (a.y() + b.y()));
        double k = std::abs(b.y() - a.y()) > M_E
                ? (a.x() + b.x()) * (r / d - 0.5) * 8.0 / 3.0 / (b.y() - a.y())
                : (a.y() + b.y()) * (r / d - 0.5) * 8.0 / 3.0 / (a.x() - b.x());

        auto startPoint = centerPoint + a;
        auto endPoint = centerPoint + b;

        auto secondPoint = startPoint + Vector2dd(-a.y(), a.x()) * k;
        auto thirdPoint = endPoint + Vector2dd(b.y(), -b.x()) * k;

        startPoint = attrs->getDrawingValues(startPoint);
        secondPoint = attrs->getDrawingValues(secondPoint);
        thirdPoint = attrs->getDrawingValues(thirdPoint);
        endPoint = attrs->getDrawingValues(endPoint);

        WuRasterizer rast = WuRasterizer();
        BezierRasterizer<RGB24Buffer, WuRasterizer> bezier(*buffer, rast, data.rgbColor);
        bezier.cubicBezierCasteljauApproximationByFlatness(Curve({startPoint, secondPoint, thirdPoint, endPoint}));
    }
}

void DxfEllipticalArcEntity::draw(class corecvs::RGB24Buffer *buffer, class corecvs::DxfDrawingAttrs *attrs) {
    auto centerPoint = data.center.xy();
    auto delta = data.majorAxisEndPoint.xy();
    auto a = delta.l2Metric();
    auto b = a * data.ratio;

    double rotationAngle = 0;
    if (delta.x() == 0) rotationAngle = M_PI / 2;
    else if (delta.y() != 0) {
        auto tan = delta.y() / delta.x();
        rotationAngle = std::atan(tan);
    }
    double cosRotationAngle = std::cos(rotationAngle);
    double sinRotationAngle = std::sin(rotationAngle);
    auto startAngle = data.startAngle;
    auto endAngle = data.endAngle;
    while (endAngle < startAngle) endAngle += 2 * M_PI;

    // divide arc into 1 or 2 segments
    std::vector<Vector2dd> controlAngles = {Vector2dd(startAngle, std::min(endAngle, startAngle + M_PI - 0.00001))};
    if (endAngle - startAngle > M_PI) controlAngles.emplace_back(Vector2dd(startAngle + M_PI, endAngle));

    // draw 1 or 2 segments
    for (Vector2dd alpha : controlAngles) {
        auto cosStartAlpha = std::cos(alpha.x());
        auto sinStartAlpha = std::sin(alpha.x());
        auto cosEndAlpha = std::cos(alpha.y());
        auto sinEndAlpha = std::sin(alpha.y());

        auto startDelta = Vector2dd(a * cosStartAlpha, b * sinStartAlpha);
        auto startPoint = centerPoint + Vector2dd(startDelta.x() * cosRotationAngle - startDelta.y() * sinRotationAngle, startDelta.x() * sinRotationAngle + startDelta.y() * cosRotationAngle);

        auto endDelta = Vector2dd(a * cosEndAlpha, b * sinEndAlpha);
        auto endPoint = centerPoint + Vector2dd(endDelta.x() * cosRotationAngle - endDelta.y() * sinRotationAngle, endDelta.x() * sinRotationAngle + endDelta.y() * cosRotationAngle);

        auto theta = std::sin(alpha.y() - alpha.x()) * (std::sqrt(4 + 3 * std::pow(std::tan((alpha.y() - alpha.x()) / 2), 2) - 1) / 3);
        auto secondPoint = startPoint + theta * Vector2dd(-a * cosRotationAngle * sinStartAlpha - b * sinRotationAngle * cosStartAlpha, -a * sinRotationAngle * sinStartAlpha + b * cosRotationAngle * cosStartAlpha);
        auto thirdPoint = endPoint - theta * Vector2dd(-a * cosRotationAngle * sinEndAlpha - b * sinRotationAngle * cosEndAlpha, -a * sinRotationAngle * sinEndAlpha + b * cosRotationAngle * cosEndAlpha);

        startPoint  = attrs->getDrawingValues(startPoint);
        secondPoint = attrs->getDrawingValues(secondPoint);
        thirdPoint  = attrs->getDrawingValues(thirdPoint);
        endPoint    = attrs->getDrawingValues(endPoint);

        WuRasterizer rast = WuRasterizer();
        BezierRasterizer<RGB24Buffer, WuRasterizer> bezier(*buffer, rast, data.rgbColor);
        bezier.cubicBezierCasteljauApproximationByFlatness(Curve({startPoint, secondPoint, thirdPoint, endPoint}));
    }
}

void DxfPointEntity::draw(class corecvs::RGB24Buffer *buffer, class corecvs::DxfDrawingAttrs *attrs) {
    auto centerPoint = attrs->getDrawingValues(data.location.x(), data.location.y());
    auto thickness = attrs->getDrawingValue(data.thickness);
    if (thickness == 0) {
        buffer->drawPixel(centerPoint, data.rgbColor);
    } else {
        AbstractPainter<RGB24Buffer> painter(buffer);
        painter.drawCircle(centerPoint, thickness / 2, data.rgbColor);
    }
}

// Bounding box getting
std::pair<Vector2dd,Vector2dd> DxfLineEntity::getBoundingBox() {
    auto lowerLeftCorner  = Vector2dd(std::min(data.startPoint.x(), data.endPoint.x()), std::min(data.startPoint.y(), data.endPoint.y()));
    auto upperRightCorner = Vector2dd(std::max(data.startPoint.x(), data.endPoint.x()), std::max(data.startPoint.y(), data.endPoint.y()));
    return std::make_pair(lowerLeftCorner, upperRightCorner);
}

std::pair<Vector2dd,Vector2dd> DxfLwPolylineEntity::getBoundingBox() {
    if (!data.vertices.empty()) {
        auto lowerLeftCorner = Vector2dd(data.vertices[0].x(), data.vertices[0].y());
        auto upperRightCorner = lowerLeftCorner;
        for (size_t i = 1; i < data.vertices.size(); i++) {
            auto x = data.vertices[i].x();
            auto y = data.vertices[i].y();
            if (x < lowerLeftCorner.x()) lowerLeftCorner.x() = x;
            else if (x > upperRightCorner.x()) upperRightCorner.x() = x;
            if (y < lowerLeftCorner.y()) lowerLeftCorner.y() = y;
            else if (y > upperRightCorner.y()) upperRightCorner.y() = y;
        }
        return std::make_pair(lowerLeftCorner, upperRightCorner);
    }
    /* We should return something empty, like Rectangled::Empty()*/
    return std::make_pair(Vector2dd::Zero(), Vector2dd::Zero());
}

std::pair<Vector2dd,Vector2dd> DxfPolylineEntity::getBoundingBox() {
    if (!data.vertices.empty()) {
        auto lowerLeftCorner = Vector2dd(data.vertices[0]->location.x(), data.vertices[0]->location.y());
        auto upperRightCorner = lowerLeftCorner;
        for (size_t i = 1; i < data.vertices.size(); i++) {
            auto x = data.vertices[i]->location.x();
            auto y = data.vertices[i]->location.y();
            if (x < lowerLeftCorner.x()) lowerLeftCorner.x() = x;
            else if (x > upperRightCorner.x()) upperRightCorner.x() = x;
            if (y < lowerLeftCorner.y()) lowerLeftCorner.y() = y;
            else if (y > upperRightCorner.y()) upperRightCorner.y() = y;
        }
        return std::make_pair(lowerLeftCorner, upperRightCorner);
    }
    return std::make_pair(Vector2dd::Zero(), Vector2dd::Zero());
}

std::pair<Vector2dd,Vector2dd> DxfCircleEntity::getBoundingBox() {
    auto lowerLeftCorner = Vector2dd(data.center.x() - data.radius, data.center.y() - data.radius);
    auto upperRightCorner = Vector2dd(data.center.x() + data.radius, data.center.y() + data.radius);
    return std::make_pair(lowerLeftCorner, upperRightCorner);
}

std::pair<Vector2dd,Vector2dd> DxfCircularArcEntity::getBoundingBox() {
    auto lowerLeftCorner = Vector2dd(data.center.x() - data.radius, data.center.y() - data.radius);
    auto upperRightCorner = Vector2dd(data.center.x() + data.radius, data.center.y() + data.radius);
    return std::make_pair(lowerLeftCorner, upperRightCorner);
}

std::pair<Vector2dd,Vector2dd> DxfEllipticalArcEntity::getBoundingBox() {
    auto majorRadius = std::sqrt(data.majorAxisEndPoint.x() * data.majorAxisEndPoint.x() + data.majorAxisEndPoint.y() * data.majorAxisEndPoint.y());
    auto lowerLeftCorner = Vector2dd(data.center.x() - majorRadius, data.center.y() - majorRadius);
    auto upperRightCorner = Vector2dd(data.center.x() + majorRadius, data.center.y() + majorRadius);
    return std::make_pair(lowerLeftCorner, upperRightCorner);
}

std::pair<Vector2dd,Vector2dd> DxfPointEntity::getBoundingBox() {
    auto lowerLeftCorner = Vector2dd(data.location.x(), data.location.y());
    auto upperRightCorner = lowerLeftCorner;
    return std::make_pair(lowerLeftCorner, upperRightCorner);
}

} // namespace corecvs