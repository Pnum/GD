/**

GDevelop - Primitive Drawing Extension
Copyright (c) 2008-2015 Florian Rival (Florian.Rival@gmail.com)
This project is released under the MIT License.
*/
#include <SFML/Graphics.hpp>
#include "GDCpp/RuntimeScene.h"
#include "GDCpp/RuntimeGame.h"
#include "GDCpp/Project.h"
#include "GDCpp/ImageManager.h"
#include "GDCpp/CommonTools.h"
#include "PrimitiveDrawingTools.h"

namespace GDpriv
{
namespace PrimitiveDrawingTools
{

void GD_EXTENSION_API CopyImageOnAnother( const std::string & destName, const std::string & srcName, float destX, float destY, bool useTransparency,RuntimeScene & scene )
{
    if ( !scene.GetImageManager()->HasLoadedSFMLTexture(destName) ) return;
    if ( !scene.GetImageManager()->HasLoadedSFMLTexture(srcName) ) return;

    boost::shared_ptr<SFMLTextureWrapper> dest = scene.GetImageManager()->GetSFMLTexture(destName);

    //Make sure the coordinates are correct.
    if ( destX < 0 || static_cast<unsigned>(destX) >= dest->texture.getSize().x) return;
    if ( destY < 0 || static_cast<unsigned>(destY) >= dest->texture.getSize().y) return;

    dest->image.copy(scene.GetImageManager()->GetSFMLTexture(srcName)->image, destX, destY, sf::IntRect(0, 0, 0, 0), useTransparency);
    dest->texture.loadFromImage(dest->image);
}

void GD_EXTENSION_API CaptureScreen( RuntimeScene & scene, const std::string & destFileName, const std::string & destImageName )
{
    if ( !scene.renderWindow ) return;
    sf::Image capture = scene.renderWindow->capture();

    if ( !destFileName.empty() ) capture.saveToFile(destFileName);
    if ( !destImageName.empty() && scene.GetImageManager()->HasLoadedSFMLTexture(destImageName) )
    {
        boost::shared_ptr<SFMLTextureWrapper> sfmlTexture = scene.GetImageManager()->GetSFMLTexture(destImageName);
        sfmlTexture->image = capture;
        sfmlTexture->texture.loadFromImage(sfmlTexture->image); //Do not forget to update the associated texture
    }
}

namespace
{

}

void GD_EXTENSION_API CreateSFMLTexture( RuntimeScene & scene, const std::string & imageName, unsigned int width, unsigned int height, const std::string & colorStr )
{
    //Get or create the texture in memory
    boost::shared_ptr<SFMLTextureWrapper> newTexture;
    if ( !scene.GetImageManager()->HasLoadedSFMLTexture(imageName) )
        newTexture = boost::shared_ptr<SFMLTextureWrapper>(new SFMLTextureWrapper);
    else
        newTexture = scene.GetImageManager()->GetSFMLTexture(imageName);

    //Get the color
    sf::Color color;
    bool colorIsOk = false;
    std::vector < std::string > colors = SplitString<std::string>(colorStr, ';');
    if ( colors.size() == 3 )
    {
        colorIsOk = true;
        color = sf::Color(ToInt(colors[0]), ToInt(colors[1]), ToInt(colors[2]));
    }

    //Create the SFML image and the SFML texture
    if ( width != 0 && height != 0 && colorIsOk )
        newTexture->image.create(width, height, color);

    newTexture->texture.loadFromImage(newTexture->image); //Do not forget to update the associated texture

    scene.GetImageManager()->SetSFMLTextureAsPermanentlyLoaded(imageName, newTexture); //Otherwise
}

void GD_EXTENSION_API OpenSFMLTextureFromFile( RuntimeScene & scene, const std::string & fileName, const std::string & imageName )
{
    //Get or create the texture in memory
    boost::shared_ptr<SFMLTextureWrapper> newTexture;
    if ( !scene.GetImageManager()->HasLoadedSFMLTexture(imageName) )
        newTexture = boost::shared_ptr<SFMLTextureWrapper>(new SFMLTextureWrapper);
    else
        newTexture = scene.GetImageManager()->GetSFMLTexture(imageName);

    //Open the SFML image and the SFML texture
    newTexture->image.loadFromFile(fileName);
    newTexture->texture.loadFromImage(newTexture->image); //Do not forget to update the associated texture

    scene.GetImageManager()->SetSFMLTextureAsPermanentlyLoaded(imageName, newTexture);
}

void GD_EXTENSION_API SaveSFMLTextureToFile( RuntimeScene & scene, const std::string & fileName, const std::string & imageName )
{
    if ( !scene.GetImageManager()->HasLoadedSFMLTexture(imageName) ) return;

    scene.GetImageManager()->GetSFMLTexture(imageName)->image.saveToFile(fileName);
}

}
}

