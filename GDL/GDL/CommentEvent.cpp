/**
 *  Game Develop
 *  2008-2010 Florian Rival (Florian.Rival@gmail.com)
 */

#include "CommentEvent.h"
#include "GDL/OpenSaveGame.h"
#include "GDL/EventsRenderingHelper.h"
#include "tinyxml.h"

void CommentEvent::SaveToXml(TiXmlElement * eventElem) const
{
    TiXmlElement * color;
    color = new TiXmlElement( "Couleur" );
    eventElem->LinkEndChild( color );

    color->SetDoubleAttribute( "r", r );
    color->SetDoubleAttribute( "v", v );
    color->SetDoubleAttribute( "b", b );

    TiXmlElement * com1Elem = new TiXmlElement( "Com1" );
    eventElem->LinkEndChild( com1Elem );
    com1Elem->SetAttribute( "value", com1.c_str() );

    TiXmlElement * com2Elem = new TiXmlElement( "Com2" );
    eventElem->LinkEndChild( com2Elem );
    com2Elem->SetAttribute( "value", com2.c_str() );
}

void CommentEvent::LoadFromXml(const TiXmlElement * eventElem)
{
    if ( eventElem->FirstChildElement( "Couleur" )->Attribute( "r" ) != NULL ) { int value;eventElem->FirstChildElement( "Couleur" )->QueryIntAttribute( "r", &value ); r = value;}
    else { cout << "Les informations concernant la couleur d'un commentaire manquent."; }
    if ( eventElem->FirstChildElement( "Couleur" )->Attribute( "v" ) != NULL ) { int value;eventElem->FirstChildElement( "Couleur" )->QueryIntAttribute( "v", &value ); v = value;}
    else { cout <<"Les informations concernant la couleur d'un commentaire manquent." ; }
    if ( eventElem->FirstChildElement( "Couleur" )->Attribute( "b" ) != NULL ) { int value;eventElem->FirstChildElement( "Couleur" )->QueryIntAttribute( "b", &value ); b = value;}
    else { cout <<"Les informations concernant la couleur d'un commentaire manquent." ; }
    if ( eventElem->FirstChildElement( "Com1" )->Attribute( "value" ) != NULL ) { com1 = eventElem->FirstChildElement( "Com1" )->Attribute( "value" );}
    else { cout <<"Les informations concernant le texte 1 d'un commentaire manquent." ; }
    if ( eventElem->FirstChildElement( "Com2" )->Attribute( "value" ) != NULL ) { com2 = eventElem->FirstChildElement( "Com2" )->Attribute( "value" );}
    else { cout <<"Les informations concernant le texte 2 d'un commentaire manquent." ; }
}

#if defined(GDE)

/**
 * Render the event in the bitmap
 */
void CommentEvent::RenderInBitmap() const
{
    EventsRenderingHelper * renderingHelper = EventsRenderingHelper::getInstance();

    //Get sizes and recreate the bitmap
    renderedEventBitmap.Create(renderedWidth, CalculateNecessaryHeight(), -1);

    //Prepare DC and constants
    wxMemoryDC dc;
    dc.SelectObject(renderedEventBitmap);
    const int sideSeparation = 3; //Espacement en pixel entre le texte et la bordure
    dc.SetFont( renderingHelper->GetFont() );

    wxRect rectangle = wxRect(dc.GetMultiLineTextExtent(com1));
    wxRect rectangle2 = wxRect(dc.GetMultiLineTextExtent(com2));

    //Setup the background
    rectangle.SetWidth(renderedWidth);
    if ( rectangle.GetHeight() >= rectangle2.GetHeight() )
        rectangle.SetHeight(rectangle.GetHeight()+sideSeparation*2);
    else
        rectangle.SetHeight(rectangle2.GetHeight()+sideSeparation*2);

    if ( !selected )
    {
        dc.SetBrush(wxBrush(wxColour(r, v, b)));
        dc.SetPen(wxPen(wxColour(r/2, v/2, b/2), 1));
    }
    else
    {
        dc.SetPen(renderingHelper->GetSelectedRectangleOutlinePen());
        dc.SetBrush(renderingHelper->GetSelectedRectangleFillBrush());
    }

    //Draw the background
    dc.DrawRectangle(rectangle);

    //Draw the text
    wxRect texteRect = rectangle;
    texteRect.SetY(texteRect.GetY()+sideSeparation);
    texteRect.SetX(texteRect.GetX()+sideSeparation);
    dc.DrawLabel( com1, texteRect );

    //Optional text
    if ( com2 != "" )
    {
        texteRect.SetX(texteRect.GetX()+renderedWidth/2);
        dc.DrawLabel( com2, texteRect );
    }
}

unsigned int CommentEvent::CalculateNecessaryHeight() const
{
    EventsRenderingHelper * renderingHelper = EventsRenderingHelper::getInstance();

    wxMemoryDC dc;
    dc.SelectObject(renderedEventBitmap);

    dc.SetFont( renderingHelper->GetFont() );
    const int sideSeparation = 3; //Espacement en pixel entre le texte et la bordure

    wxRect rectangle = wxRect(dc.GetMultiLineTextExtent(com1));
    wxRect rectangle2 = wxRect(dc.GetMultiLineTextExtent(com2));

    if ( rectangle.GetHeight() >= rectangle2.GetHeight() )
        rectangle.SetHeight(rectangle.GetHeight()+sideSeparation*2);
    else
        rectangle.SetHeight(rectangle2.GetHeight()+sideSeparation*2);

    return rectangle.GetHeight();
}
#endif