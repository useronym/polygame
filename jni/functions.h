#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include <irrlicht.h>

using namespace irr;
using namespace core;

/*inline void scaleGUIPos(position2d<s32> &p, vector2df gscale)
{
    p.X = p.X * gscale.X;
    p.Y = p.Y * gscale.Y;
}

inline void scaleGUIRect(rect<s32> &r, vector2df gscale)
{
    scaleGUIPos(r.LowerRightCorner, gscale);
    scaleGUIPos(r.UpperLeftCorner, gscale);
}

inline gui::IGUIButton* addButton(rect<s32> rect, video::ITexture* tex, SContext* cont, s32 id = -1, gui::IGUIElement* parent = 0)
{
    scaleGUIRect(rect, cont->GUIScale);
    gui::IGUIButton* btn = cont->Device->getGUIEnvironment()->addButton(rect, parent, id);
    btn->setDrawBorder(false);
    btn->setImage(tex);
    btn->setScaleImage(true);
    btn->setUseAlphaChannel(true);

    return btn;
}*/


#endif // FUNCTIONS_H_INCLUDED
