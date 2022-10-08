// Copyright(c) 2019 - 2022, KaoruXun All rights reserved.

#include "DebugImpl.hpp"

#include <cstddef>
#include <exception>
#include <string>

namespace MetaEngine {
    static const char *date = __DATE__;
    static const char *mon[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    static const char mond[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    int meko_buildnum(void) {
        int m = 0, d = 0, y = 0;
        static int b = 0;

        if (b != 0)
            return b;

        for (m = 0; m < 11; m++) {
            if (!strncmp(&date[0], mon[m], 3))
                break;
            d += mond[m];
        }

        d += atoi(&date[4]) - 1;
        y = atoi(&date[7]) - 2000;
        b = d + (int) ((y - 1) * 365.25f);

        if (((y % 4) == 0) && m > 1) {
            b += 1;
        }
        b -= 7340;

        return b;
    }
}// namespace MetaEngine


b2DebugDraw_impl::b2DebugDraw_impl(METAENGINE_Render_Target *target) {
    this->target = target;
}

b2DebugDraw_impl::~b2DebugDraw_impl() {}

void b2DebugDraw_impl::Create() {}

void b2DebugDraw_impl::Destroy() {}

b2Vec2 b2DebugDraw_impl::transform(const b2Vec2 &pt) {
    float x = ((pt.x) * scale + xOfs);
    float y = ((pt.y) * scale + yOfs);
    return b2Vec2(x, y);
}

SDL_Color b2DebugDraw_impl::convertColor(const b2Color &color) {
    return {(Uint8) (color.r * 255), (Uint8) (color.g * 255), (Uint8) (color.b * 255), (Uint8) (color.a * 255)};
}

void b2DebugDraw_impl::DrawPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color) {
    b2Vec2 *verts = new b2Vec2[vertexCount];

    for (int i = 0; i < vertexCount; i++) {
        verts[i] = transform(vertices[i]);
    }

    // the "(float*)verts" assumes a b2Vec2 is equal to two floats (which it is)
    METAENGINE_Render_Polygon(target, vertexCount, (float *) verts, convertColor(color));

    delete[] verts;
}

void b2DebugDraw_impl::DrawSolidPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color) {
    b2Vec2 *verts = new b2Vec2[vertexCount];

    for (int i = 0; i < vertexCount; i++) {
        verts[i] = transform(vertices[i]);
    }

    // the "(float*)verts" assumes a b2Vec2 is equal to two floats (which it is)
    SDL_Color c2 = convertColor(color);
    c2.a *= 0.25;
    METAENGINE_Render_PolygonFilled(target, vertexCount, (float *) verts, c2);
    METAENGINE_Render_Polygon(target, vertexCount, (float *) verts, convertColor(color));

    delete[] verts;
}

void b2DebugDraw_impl::DrawCircle(const b2Vec2 &center, float radius, const b2Color &color) {
    b2Vec2 tr = transform(center);
    METAENGINE_Render_Circle(target, tr.x, tr.y, radius * scale, convertColor(color));
}

void b2DebugDraw_impl::DrawSolidCircle(const b2Vec2 &center, float radius, const b2Vec2 &axis, const b2Color &color) {
    b2Vec2 tr = transform(center);
    METAENGINE_Render_CircleFilled(target, tr.x, tr.y, radius * scale, convertColor(color));
}

void b2DebugDraw_impl::DrawSegment(const b2Vec2 &p1, const b2Vec2 &p2, const b2Color &color) {
    b2Vec2 tr1 = transform(p1);
    b2Vec2 tr2 = transform(p2);
    METAENGINE_Render_Line(target, tr1.x, tr1.y, tr2.x, tr2.y, convertColor(color));
}

void b2DebugDraw_impl::DrawTransform(const b2Transform &xf) {
    const float k_axisScale = 8.0f;
    b2Vec2 p1 = xf.p, p2;
    b2Vec2 tr1 = transform(p1), tr2;

    p2 = p1 + k_axisScale * xf.q.GetXAxis();
    tr2 = transform(p2);
    METAENGINE_Render_Line(target, tr1.x, tr1.y, tr2.x, tr2.y, {0xff, 0x00, 0x00, 0xcc});

    p2 = p1 + k_axisScale * xf.q.GetYAxis();
    tr2 = transform(p2);
    METAENGINE_Render_Line(target, tr1.x, tr1.y, tr2.x, tr2.y, {0x00, 0xff, 0x00, 0xcc});
}

void b2DebugDraw_impl::DrawPoint(const b2Vec2 &p, float size, const b2Color &color) {
    b2Vec2 tr = transform(p);
    METAENGINE_Render_CircleFilled(target, tr.x, tr.y, 2, convertColor(color));
}

void b2DebugDraw_impl::DrawString(int x, int y, const char *string, ...) {}

void b2DebugDraw_impl::DrawString(const b2Vec2 &p, const char *string, ...) {}

void b2DebugDraw_impl::DrawAABB(b2AABB *aabb, const b2Color &color) {
    b2Vec2 tr1 = transform(aabb->lowerBound);
    b2Vec2 tr2 = transform(aabb->upperBound);
    METAENGINE_Render_Line(target, tr1.x, tr1.y, tr2.x, tr1.y, convertColor(color));
    METAENGINE_Render_Line(target, tr2.x, tr1.y, tr2.x, tr2.y, convertColor(color));
    METAENGINE_Render_Line(target, tr2.x, tr2.y, tr1.x, tr2.y, convertColor(color));
    METAENGINE_Render_Line(target, tr1.x, tr2.y, tr1.x, tr1.y, convertColor(color));
}
