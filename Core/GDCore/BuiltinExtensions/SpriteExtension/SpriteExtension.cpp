/*
 * GDevelop Core
 * Copyright 2008-2015 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the GNU Lesser General Public LicenFse.
 */
#include "GDCore/BuiltinExtensions/AllBuiltinExtensions.h"
#include "GDCore/Tools/Localization.h"
#include "GDCore/PlatformDefinition/Object.h"
#include "GDCore/BuiltinExtensions/SpriteExtension/SpriteObject.h"

using namespace std;
namespace gd
{

void GD_CORE_API BuiltinExtensionsImplementer::ImplementsSpriteExtension(gd::PlatformExtension & extension)
{
    extension.SetExtensionInformation("Sprite",
                          _("Sprite"),
                          _("Extension for adding animated objects in the scene, which can contain animations with directions within each."),
                          "Florian Rival",
                          "Open source (MIT License)");

    gd::ObjectMetadata & obj = extension.AddObject("Sprite",
               _("Sprite"),
               _("Animated object which can be used for most elements of a game"),
               "CppPlatform/Extensions/spriteicon.png",
               &CreateSpriteObject);

    #if defined(GD_IDE_ONLY)
    obj.AddAction("Opacity",
                   _("Change object's opacity"),
                   _("Change the opacity of an object."),
                   _("Do _PARAM1__PARAM2_ to the opacity of _PARAM0_"),
                   _("Visibility"),
                   "res/actions/opacity24.png",
                   "res/actions/opacity.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("operator", _("Modification's sign"), "",false)
        .AddParameter("expression", _("Value"), "",false)
        .MarkAsSimple()
        .SetManipulatedType("number");

    obj.AddAction("ChangeAnimation",
                   _("Change the animation"),
                   _("Modify the current animation of the object."),
                   _("Do _PARAM1__PARAM2_ to the number of current animation of _PARAM0_"),
                   _("Animations and images"),
                   "res/actions/animation24.png",
                   "res/actions/animation.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("operator", _("Modification's sign"), "",false)
        .AddParameter("expression", _("Value"), "",false)
        .MarkAsSimple()
        .SetManipulatedType("number");

    obj.AddAction("ChangeDirection",
                   _("Change the direction"),
                   _("Change the direction of the object.\nIf the object is set to automatically rotate, the direction is its angle.\nIf the object is in 8 directions mode, the valid directions are 0..7"),
                   _("Do _PARAM1__PARAM2_ to the direction of _PARAM0_"),
                   _("Direction"),
                   "res/actions/direction24.png",
                   "res/actions/direction.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("operator", _("Modification's sign"), "",false)
        .AddParameter("expression", _("Value"), "",false)
        .MarkAsAdvanced()
        .SetManipulatedType("number");


    obj.AddAction("ChangeSprite",
                   _("Current frame"),
                   _("Modify the current frame of the object"),
                   _("Do _PARAM1__PARAM2_ to animation frame of _PARAM0_"),
                   _("Animations and images"),
                   "res/actions/sprite24.png",
                   "res/actions/sprite.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("operator", _("Modification's sign"), "",false)
        .AddParameter("expression", _("Value"), "",false)
        .MarkAsAdvanced()
        .SetManipulatedType("number");

    obj.AddAction("PauseAnimation",
                   _("Pause the animation"),
                   _("Pause the current animation of the object"),
                   _("Pause the current animation of _PARAM0_"),
                   _("Animations and images"),
                   "res/actions/animation24.png",
                   "res/actions/animation.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .MarkAsSimple();


    obj.AddAction("PlayAnimation",
                   _("Play the animation"),
                   _("Play the current animation of the object"),
                   _("Play the current animation of _PARAM0_"),
                   _("Animations and images"),
                   "res/actions/animation24.png",
                   "res/actions/animation.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .MarkAsSimple();

    obj.AddAction("TourneVersPos",
                   _("Rotate an object toward a position"),
                   _("Rotate an object towards a position."),
                   _("Rotate _PARAM0_ towards _PARAM1_;_PARAM2_"),
                   _("Direction"),
                   "res/actions/direction24.png",
                   "res/actions/direction.png")

        .AddParameter("object", _("Object to be rotated"), "Sprite", false)
        .AddParameter("expression", _("X position"), "",false)
        .AddParameter("expression", _("Y position"), "",false)
        .AddParameter("expression", _("Angular speed (degrees per second)"), "",false).SetDefaultValue("0")
        .AddCodeOnlyParameter("currentScene", "")
        .SetHidden(); //Deprecated


    obj.AddAction("ChangeScale",
                   _("Scale"),
                   _("Modify the scale of the specified object."),
                   _("Do _PARAM1__PARAM2_ to the scale of _PARAM0_"),
                   _("Size"),
                   "res/actions/scale24.png",
                   "res/actions/scale.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("operator", _("Modification's sign"), "",false)
        .AddParameter("expression", _("Value"), "",false)
        .MarkAsAdvanced();


    obj.AddAction("ChangeScaleWidth",
                   _("Scale on X axis"),
                   _("Modify the scale of the width of an object."),
                   _("Do _PARAM1__PARAM2_ to the width's scale of _PARAM0_"),
                   _("Size"),
                   "res/actions/scale24.png",
                   "res/actions/scale.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("operator", _("Modification's sign"), "",false)
        .AddParameter("expression", _("Value"), "",false)
        .MarkAsAdvanced()
        .SetManipulatedType("number");


    obj.AddAction("ChangeScaleHeight",
                   _("Scale on Y axis"),
                   _("Modify the scale of the height of an object."),
                   _("Do _PARAM1__PARAM2_ to the height's scale of _PARAM0_"),
                   _("Size"),
                   "res/actions/scale24.png",
                   "res/actions/scale.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("operator", _("Modification's sign"), "",false)
        .AddParameter("expression", _("Value"), "",false)
        .MarkAsAdvanced()
        .SetManipulatedType("number");


    obj.AddCondition("Animation",
                   _("Current animation"),
                   _("Test the number of the current animation of the object."),
                   _("The number of the current animation of _PARAM0_ is _PARAM1__PARAM2_"),
                   _("Animations and images"),
                   "res/conditions/animation24.png",
                   "res/conditions/animation.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("relationalOperator", _("Sign of the test"), "",false)
        .AddParameter("expression", _("Number to test"), "",false)
        .MarkAsAdvanced()
        .SetManipulatedType("number");

    obj.AddCondition("Direction",
                   _("Current direction"),
                   _("Compare the direction of the object. If 8 direction mode is activated for the sprite, the value taken for direction will be from 0 to 7. Otherwise, the direction is in degrees."),
                   _("Direction of _PARAM0_ is _PARAM1__PARAM2_"),
                   _("Direction"),
                   "res/conditions/direction24.png",
                   "res/conditions/direction.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("relationalOperator", _("Sign of the test"), "",false)
        .AddParameter("expression", _("Direction to test"), "",false)
        .SetManipulatedType("number");

    obj.AddCondition("Sprite",
                   _("Current frame"),
                   _("Test the number of the current animation frame."),
                   _("The animation frame of _PARAM0_ is _PARAM1__PARAM2_"),
                   _("Animations and images"),
                   "res/conditions/sprite24.png",
                   "res/conditions/sprite.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("relationalOperator", _("Sign of the test"), "",false)
        .AddParameter("expression", _("Animation frame to test"), "",false)
        .MarkAsAdvanced()
        .SetManipulatedType("number");

    obj.AddCondition("AnimStopped",
                   _("Animation paused"),
                   _("Test if the animation of an object is paused"),
                   _("The animation of _PARAM0_ is paused"),
                   _("Animations and images"),
                   "res/conditions/animation24.png",
                   "res/conditions/animation.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .MarkAsSimple();

    obj.AddCondition("AnimationEnded",
                   _("Animation finished"),
                   _("Check if the animation being played by the Sprite object is finished."),
                   _("The animation of _PARAM0_ is finished"),
                   _("Animations and images"),
                   "res/conditions/animation24.png",
                   "res/conditions/animation.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .MarkAsSimple();

    obj.AddCondition("ScaleWidth",
                   _("Scale on X axis"),
                   _("Compare the scale of the width of an object."),
                   _("The width's scale of _PARAM0_ is _PARAM1__PARAM2_"),
                   _("Size"),
                   "res/conditions/scaleWidth24.png",
                   "res/conditions/scaleWidth.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("relationalOperator", _("Sign of the test"), "",false)
        .AddParameter("expression", _("Value to test"), "",false)
        .MarkAsAdvanced()
        .SetManipulatedType("number");

    obj.AddCondition("ScaleHeight",
                   _("Scale on Y axis"),
                   _("Compare the scale of the height of an object."),
                   _("The height's scale of _PARAM0_ is _PARAM1__PARAM2_"),
                   _("Size"),
                   "res/conditions/scaleHeight24.png",
                   "res/conditions/scaleHeight.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("relationalOperator", _("Sign of the test"), "",false)
        .AddParameter("expression", _("Value to test"), "",false)
        .MarkAsAdvanced()
        .SetManipulatedType("number");

    obj.AddCondition("Opacity",
                   _("Opacity"),
                   _("Compare the opacity of an object, between 0 (fully transparent) to 255 (opaque)"),
                   _("The opacity of _PARAM0_ is _PARAM1__PARAM2_"),
                   _("Visibility"),
                   "res/conditions/opacity24.png",
                   "res/conditions/opacity.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("relationalOperator", _("Sign of the test"), "",false)
        .AddParameter("expression", _("Value to test"), "",false)
        .MarkAsSimple()
        .SetManipulatedType("number");

    obj.AddCondition("BlendMode",
                   _("Blend mode"),
                   _("Compare the number of the blend mode currently used by an object"),
                   _("The number of the current blend mode of _PARAM0_ is _PARAM1__PARAM2_"),
                   _("Effects"),
                   "res/conditions/opacity24.png",
                   "res/conditions/opacity.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("relationalOperator", _("Sign of the test"), "",false)
        .AddParameter("expression", _("Value to test  ( 0 : Alpha, 1 : Add, 2 : Multiply, 3 : None )"), "",false)
        .MarkAsAdvanced()
        .SetManipulatedType("number");

    obj.AddAction("CopyImageOnImageOfSprite",
                   _("Copy an image on the current one of an object"),
                   _("Copy an image on the current image of an object.\nNote that the source image must be preferably kept loaded in memory."),
                   _("Copy image _PARAM2_ on the current of _PARAM0_ at _PARAM3_;_PARAM4_"),
                   _("Effects"),
                   "res/copy24.png",
                   "res/copyicon.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddCodeOnlyParameter("currentScene", "")
        .AddParameter("string", _("Name of the source image"), "",false)
        .AddParameter("expression", _("X position"), "",false)
        .AddParameter("expression", _("Y position"), "",false)
        .AddParameter("yesorno", _("Should the copy take in account the source transparency\?"), "",false);

    obj.AddAction("CreateMaskFromColorOnActualImage", //Actual is indeed a mistake : Current should have been chosen.
                   _("Make a color of the image of an object transparent"),
                   _("Make a color of the image of an object transparent."),
                   _("Make color _PARAM1_ of the current image of _PARAM0_ transparent"),
                   _("Effects"),
                   "res/actions/opacity24.png",
                   "res/actions/opacity.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("color", _("Color to make transparent"), "",false);


    obj.AddAction("ChangeColor",
                   _("Global color"),
                   _("Change the global color of an object. The default color is white."),
                   _("Change color of _PARAM0_ to _PARAM1_"),
                   _("Effects"),
                   "res/actions/color24.png",
                   "res/actions/color.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("color", _("Color"), "",false);


    obj.AddAction("ChangeBlendMode",
                   _("Blend mode"),
                   _("Change the number of the blend mode of an object.\nThe default blend mode is 0 (Alpha)."),
                   _("Change Blend mode of _PARAM0_ to _PARAM1_"),
                   _("Effects"),
                   "res/actions/color24.png",
                   "res/actions/color.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("expression", _("Mode (0 : Alpha, 1 : Add, 2 : Multiply, 3 : None)"), "",false)
        .MarkAsSimple();


    obj.AddAction("FlipX",
                   _("Flip the object horizontally"),
                   _("Flip the object horizontally"),
                   _("Flip horizontally _PARAM0_ : _PARAM1_"),
                   _("Effects"),
                   "res/actions/flipX24.png",
                   "res/actions/flipX.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("yesorno", _("Activate flipping"), "",false)
        .MarkAsSimple();


    obj.AddAction("FlipY",
                   _("Flip the object vertically"),
                   _("Flip the object vertically"),
                   _("Flip vertically _PARAM0_ : _PARAM1_"),
                   _("Effects"),
                   "res/actions/flipY24.png",
                   "res/actions/flipY.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("yesorno", _("Activate flipping"), "",false)
        .MarkAsSimple();

    obj.AddCondition("FlippedX",
                   _("Horizontally flipped"),
                   _("Return true if the object is horizontally flipped"),
                   _("_PARAM0_ is horizontally flipped"),
                   _("Effects"),
                   "res/actions/flipX24.png",
                   "res/actions/flipX.png")

        .AddParameter("object", _("Object"), "Sprite", false);

    obj.AddCondition("FlippedY",
                   _("Vertically flipped"),
                   _("Return true if the object is vertically flipped"),
                   _("_PARAM0_ is vertically flipped"),
                   _("Effects"),
                   "res/actions/flipY24.png",
                   "res/actions/flipY.png")

        .AddParameter("object", _("Object"), "Sprite", false);

    obj.AddAction("TourneVers",
                   _("Rotate an object toward another"),
                   _("Rotate an object towards another."),
                   _("Rotate _PARAM0_ towards _PARAM1_"),
                   _("Direction"),
                   "res/actions/direction24.png",
                   "res/actions/direction.png")

        .AddParameter("object", _("Object to be rotated"), "Sprite", false)
        .AddParameter("objectPtr", _("Rotate toward this object"))
        .AddCodeOnlyParameter("currentScene", "")
        .SetHidden(); //Deprecated


    obj.AddCondition("SourisSurObjet",
                   _("The cursor is on an object"),
                   _("Test if the cursor is over a Sprite object. The test is accurate by default (check that the cursor is not on a transparent pixel)."),
                   _("The cursor is on _PARAM0_"),
                   _("Mouse"),
                   "res/conditions/surObjet24.png",
                   "res/conditions/surObjet.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddCodeOnlyParameter("currentScene", "")
        .AddParameter("yesorno", _("Precise test ( yes by default )"), "", true).SetDefaultValue("yes")
        .MarkAsSimple();

    obj.AddExpression("X", _("X position of a point"), _("X position of a point"), _("Position"), "res/actions/position.png")
        .SetHidden()
        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("", _("Name of the point"), "", true);

    obj.AddExpression("Y", _("Y position of a point"), _("Y position of a point"), _("Position"), "res/actions/position.png")
        .SetHidden()
        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("", _("Name of the point"), "", true);

    obj.AddExpression("PointX", _("X position of a point"), _("X position of a point"), _("Position"), "res/actions/position.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("", _("Name of the point"), "",false);

    obj.AddExpression("PointY", _("Y position of a point"), _("Y position of a point"), _("Position"), "res/actions/position.png")

        .AddParameter("object", _("Object"), "Sprite", false)
        .AddParameter("", _("Name of the point"), "",false);

    obj.AddExpression("Direc", _("Direction"), _("Direction of the object"), _("Direction"), "res/actions/direction.png")
        .SetHidden()
        .AddParameter("object", _("Object"), "Sprite", false);

    obj.AddExpression("Direction", _("Direction"), _("Direction of the object"), _("Direction"), "res/actions/direction.png")
        .AddParameter("object", _("Object"), "Sprite", false);

    obj.AddExpression("Anim", _("Animation"), _("Animation of the object"), _("Animations and images"), "res/actions/animation.png")
        .SetHidden()
        .AddParameter("object", _("Object"), "Sprite", false);

    obj.AddExpression("Animation", _("Animation"), _("Animation of the object"), _("Animations and images"), "res/actions/animation.png")
        .AddParameter("object", _("Object"), "Sprite", false);

    obj.AddExpression("Sprite", _("Image"), _("Animation frame of the object"), _("Animations and images"), "res/actions/sprite.png")
        .AddParameter("object", _("Object"), "Sprite", false);

    obj.AddExpression("ScaleX", _("Scale of the width of an object"), _("Scale of the width of an object"), _("Size"), "res/actions/scaleWidth.png")
        .AddParameter("object", _("Object"), "Sprite", false);

    obj.AddExpression("ScaleY", _("Scale of the height of an object"), _("Scale of the height of an object"), _("Size"), "res/actions/scaleHeight.png")
        .AddParameter("object", _("Object"), "Sprite", false);

    extension.AddCondition("Collision",
                      _("Collision (Pixel perfect)"),
                      _("The condition is true if there is a collision between the two objects.\nThe test is pixel-perfect."),
                      _("_PARAM0_ is in collision with _PARAM1_ (pixel perfect)"),
                      _("Collision"),
                      "res/conditions/collision24.png",
                      "res/conditions/collision.png")
        .AddParameter("objectList", _("Object 1"), "Sprite", false)
        .AddParameter("objectList", _("Object 2"), "Sprite", false)
        .AddCodeOnlyParameter("conditionInverted", "");
    #endif
}

}
