/*
 * GDevelop Core
 * Copyright 2008-2015 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>
#include "GDCore/IDE/Dialogs/ProjectExtensionsDialog.h"
#include "GDCore/IDE/Dialogs/ProjectUpdateDialog.h"
#include "GDCore/IDE/Dialogs/ChooseVariableDialog.h"
#include "GDCore/IDE/ArbitraryResourceWorker.h"
#include "GDCore/PlatformDefinition/Platform.h"
#include "GDCore/PlatformDefinition/PlatformExtension.h"
#include "GDCore/PlatformDefinition/Layout.h"
#include "GDCore/PlatformDefinition/ExternalEvents.h"
#include "GDCore/PlatformDefinition/ExternalLayout.h"
#include "GDCore/PlatformDefinition/SourceFile.h"
#include "GDCore/PlatformDefinition/ImageManager.h"
#include "GDCore/PlatformDefinition/Object.h"
#include "GDCore/PlatformDefinition/ResourcesManager.h"
#include "GDCore/PlatformDefinition/ChangesNotifier.h"
#include "GDCore/Events/ExpressionMetadata.h"
#include "GDCore/Serialization/SerializerElement.h"
#include "GDCore/Serialization/Serializer.h"
#include "GDCore/IDE/MetadataProvider.h"
#include "GDCore/IDE/PlatformManager.h"
#include "GDCore/CommonTools.h"
#include "GDCore/TinyXml/tinyxml.h"
#include "GDCore/Tools/VersionWrapper.h"
#include "GDCore/Tools/Log.h"
#include "GDCore/Tools/Localization.h"
#include "GDCore/Serialization/Serializer.h"
#include "Project.h"
#if defined(GD_IDE_ONLY) && !defined(GD_NO_WX_GUI)
#include <wx/propgrid/propgrid.h>
#include <wx/settings.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/settings.h>
#include <wx/filename.h>
#endif

using namespace std;

namespace gd
{

Project::Project() :
    #if defined(GD_IDE_ONLY)
    name(_("Project")),
    #endif
    windowWidth(800),
    windowHeight(600),
    maxFPS(60),
    minFPS(10),
    verticalSync(false)
    #if !defined(GD_NO_WX_GUI)
    ,imageManager(boost::shared_ptr<gd::ImageManager>(new ImageManager))
    #endif
    #if defined(GD_IDE_ONLY)
    ,useExternalSourceFiles(false),
    currentPlatform(NULL),
    GDMajorVersion(gd::VersionWrapper::Major()),
    GDMinorVersion(gd::VersionWrapper::Minor()),
    dirty(false)
    #endif
{
    #if !defined(GD_NO_WX_GUI)
    imageManager->SetGame(this);
    #endif
    #if defined(GD_IDE_ONLY)
    //Game use builtin extensions by default
    extensionsUsed.push_back("BuiltinObject");
    extensionsUsed.push_back("BuiltinAudio");
    extensionsUsed.push_back("BuiltinVariables");
    extensionsUsed.push_back("BuiltinTime");
    extensionsUsed.push_back("BuiltinMouse");
    extensionsUsed.push_back("BuiltinKeyboard");
    extensionsUsed.push_back("BuiltinJoystick");
    extensionsUsed.push_back("BuiltinCamera");
    extensionsUsed.push_back("BuiltinWindow");
    extensionsUsed.push_back("BuiltinFile");
    extensionsUsed.push_back("BuiltinNetwork");
    extensionsUsed.push_back("BuiltinScene");
    extensionsUsed.push_back("BuiltinAdvanced");
    extensionsUsed.push_back("Sprite");
    extensionsUsed.push_back("BuiltinCommonInstructions");
    extensionsUsed.push_back("BuiltinCommonConversions");
    extensionsUsed.push_back("BuiltinStringInstructions");
    extensionsUsed.push_back("BuiltinMathematicalTools");
    extensionsUsed.push_back("BuiltinExternalLayouts");
    #endif

    #if !defined(GD_IDE_ONLY)
    platforms.push_back(&CppPlatform::Get());
    #endif
}

Project::~Project()
{
}

boost::shared_ptr<gd::Object> Project::CreateObject(const std::string & type, const std::string & name, const std::string & platformName)
{
    for (unsigned int i = 0;i<platforms.size();++i)
    {
        if ( !platformName.empty() && platforms[i]->GetName() != platformName ) continue;

        boost::shared_ptr<gd::Object> object = platforms[i]->CreateObject(type, name);
        if ( object ) return object;
    }

    return boost::shared_ptr<gd::Object>();
}

gd::Automatism* Project::CreateAutomatism(const std::string & type, const std::string & platformName)
{
    for (unsigned int i = 0;i<platforms.size();++i)
    {
        if ( !platformName.empty() && platforms[i]->GetName() != platformName ) continue;

        gd::Automatism* automatism = platforms[i]->CreateAutomatism(type);
        if ( automatism ) return automatism;
    }

    return NULL;
}

boost::shared_ptr<gd::AutomatismsSharedData> Project::CreateAutomatismSharedDatas(const std::string & type, const std::string & platformName)
{
    for (unsigned int i = 0;i<platforms.size();++i)
    {
        if ( !platformName.empty() && platforms[i]->GetName() != platformName ) continue;

        boost::shared_ptr<gd::AutomatismsSharedData> automatism = platforms[i]->CreateAutomatismSharedDatas(type);
        if ( automatism ) return automatism;
    }

    return boost::shared_ptr<gd::AutomatismsSharedData>();
}

#if defined(GD_IDE_ONLY)
boost::shared_ptr<gd::BaseEvent> Project::CreateEvent(const std::string & type, const std::string & platformName)
{
    for (unsigned int i = 0;i<platforms.size();++i)
    {
        if ( !platformName.empty() && platforms[i]->GetName() != platformName ) continue;

        boost::shared_ptr<gd::BaseEvent> event = platforms[i]->CreateEvent(type);
        if ( event ) return event;
    }

    return boost::shared_ptr<gd::BaseEvent>();
}

Platform & Project::GetCurrentPlatform() const
{
    if ( currentPlatform == NULL )
        std::cout << "FATAL ERROR: Project has no assigned current platform. GD will crash." << std::endl;

    return *currentPlatform;
}

void Project::AddPlatform(Platform & platform)
{
    for (unsigned int i = 0;i<platforms.size();++i)
    {
        if (platforms[i] == &platform)
            return;
    }

    //Add the platform and make it the current one if the game has no other platform.
    platforms.push_back(&platform);
    if ( currentPlatform == NULL ) currentPlatform = &platform;
}

void Project::SetCurrentPlatform(const std::string & platformName)
{
    for (unsigned int i = 0;i<platforms.size();++i)
    {
        if (platforms[i]->GetName() == platformName)
        {
            currentPlatform = platforms[i];
            return;
        }
    }
}

bool Project::RemovePlatform(const std::string & platformName)
{
    if ( platforms.size() <= 1 ) return false;

    for (unsigned int i = 0;i<platforms.size();++i)
    {
        if (platforms[i]->GetName() == platformName)
        {
            //Remove the platform, ensuring that currentPlatform remains correct.
            if ( currentPlatform == platforms[i] ) currentPlatform = platforms.back();
            if ( currentPlatform == platforms[i] ) currentPlatform = platforms[0];
            platforms.erase(platforms.begin()+i);

            return true;
        }
    }

    return false;
}
#endif

bool Project::HasLayoutNamed(const std::string & name) const
{
    return ( find_if(scenes.begin(), scenes.end(), bind2nd(gd::LayoutHasName(), name)) != scenes.end() );
}
gd::Layout & Project::GetLayout(const std::string & name)
{
    return *(*find_if(scenes.begin(), scenes.end(), bind2nd(gd::LayoutHasName(), name)));
}
const gd::Layout & Project::GetLayout(const std::string & name) const
{
    return *(*find_if(scenes.begin(), scenes.end(), bind2nd(gd::LayoutHasName(), name)));
}
gd::Layout & Project::GetLayout(unsigned int index)
{
    return *scenes[index];
}
const gd::Layout & Project::GetLayout (unsigned int index) const
{
    return *scenes[index];
}
unsigned int Project::GetLayoutPosition(const std::string & name) const
{
    for (unsigned int i = 0;i<scenes.size();++i)
    {
        if ( scenes[i]->GetName() == name ) return i;
    }
    return std::string::npos;
}
unsigned int Project::GetLayoutsCount() const
{
    return scenes.size();
}

#if defined(GD_IDE_ONLY)
void Project::SwapLayouts(unsigned int first, unsigned int second)
{
    if ( first >= scenes.size() || second >= scenes.size() )
        return;

    boost::shared_ptr<gd::Layout> firstItem = scenes[first];
    boost::shared_ptr<gd::Layout> secondItem = scenes[second];
    scenes[first] = secondItem;
    scenes[second] = firstItem;
}
#endif

gd::Layout & Project::InsertNewLayout(const std::string & name, unsigned int position)
{
    boost::shared_ptr<gd::Layout> newScene = boost::shared_ptr<gd::Layout>(new Layout);
    if (position<scenes.size())
        scenes.insert(scenes.begin()+position, newScene);
    else
        scenes.push_back(newScene);

    newScene->SetName(name);
    #if defined(GD_IDE_ONLY)
    newScene->UpdateAutomatismsSharedData(*this);
    #endif

    return *newScene;
}

gd::Layout & Project::InsertLayout(const gd::Layout & layout, unsigned int position)
{
    boost::shared_ptr<gd::Layout> newScene = boost::shared_ptr<gd::Layout>(new Layout(layout));
    if (position<scenes.size())
        scenes.insert(scenes.begin()+position, newScene);
    else
        scenes.push_back(newScene);

    #if defined(GD_IDE_ONLY)
    newScene->UpdateAutomatismsSharedData(*this);
    #endif

    return *newScene;
}

void Project::RemoveLayout(const std::string & name)
{
    std::vector< boost::shared_ptr<gd::Layout> >::iterator scene = find_if(scenes.begin(), scenes.end(), bind2nd(gd::LayoutHasName(), name));
    if ( scene == scenes.end() ) return;

    scenes.erase(scene);
}

#if defined(GD_IDE_ONLY)
bool Project::HasExternalEventsNamed(const std::string & name) const
{
    return ( find_if(externalEvents.begin(), externalEvents.end(), bind2nd(gd::ExternalEventsHasName(), name)) != externalEvents.end() );
}
gd::ExternalEvents & Project::GetExternalEvents(const std::string & name)
{
    return *(*find_if(externalEvents.begin(), externalEvents.end(), bind2nd(gd::ExternalEventsHasName(), name)));
}
const gd::ExternalEvents & Project::GetExternalEvents(const std::string & name) const
{
    return *(*find_if(externalEvents.begin(), externalEvents.end(), bind2nd(gd::ExternalEventsHasName(), name)));
}
gd::ExternalEvents & Project::GetExternalEvents(unsigned int index)
{
    return *externalEvents[index];
}
const gd::ExternalEvents & Project::GetExternalEvents (unsigned int index) const
{
    return *externalEvents[index];
}
unsigned int Project::GetExternalEventsPosition(const std::string & name) const
{
    for (unsigned int i = 0;i<externalEvents.size();++i)
    {
        if ( externalEvents[i]->GetName() == name ) return i;
    }
    return std::string::npos;
}
unsigned int Project::GetExternalEventsCount() const
{
    return externalEvents.size();
}

gd::ExternalEvents & Project::InsertNewExternalEvents(const std::string & name, unsigned int position)
{
    boost::shared_ptr<gd::ExternalEvents> newExternalEvents(new gd::ExternalEvents);
    if (position<externalEvents.size())
        externalEvents.insert(externalEvents.begin()+position, newExternalEvents);
    else
        externalEvents.push_back(newExternalEvents);

    newExternalEvents->SetName(name);
    return *newExternalEvents;
}

void Project::InsertExternalEvents(const gd::ExternalEvents & events, unsigned int position)
{
    if (position<externalEvents.size())
        externalEvents.insert(externalEvents.begin()+position, boost::shared_ptr<gd::ExternalEvents>(new gd::ExternalEvents(events)));
    else
        externalEvents.push_back(boost::shared_ptr<gd::ExternalEvents>(new gd::ExternalEvents(events)));
}

void Project::RemoveExternalEvents(const std::string & name)
{
    std::vector< boost::shared_ptr<gd::ExternalEvents> >::iterator events = find_if(externalEvents.begin(), externalEvents.end(), bind2nd(gd::ExternalEventsHasName(), name));
    if ( events == externalEvents.end() ) return;

    externalEvents.erase(events);
}

void Project::SwapExternalEvents(unsigned int first, unsigned int second)
{
    if ( first >= externalEvents.size() || second >= externalEvents.size() )
        return;

    boost::shared_ptr<gd::ExternalEvents> firstItem = externalEvents[first];
    boost::shared_ptr<gd::ExternalEvents> secondItem = externalEvents[second];
    externalEvents[first] = secondItem;
    externalEvents[second] = firstItem;
}

void Project::SwapExternalLayouts(unsigned int first, unsigned int second)
{
    if ( first >= externalLayouts.size() || second >= externalLayouts.size() )
        return;

    boost::shared_ptr<gd::ExternalLayout> firstItem = externalLayouts[first];
    boost::shared_ptr<gd::ExternalLayout> secondItem = externalLayouts[second];
    externalLayouts[first] = secondItem;
    externalLayouts[second] = firstItem;
}
#endif
bool Project::HasExternalLayoutNamed(const std::string & name) const
{
    return ( find_if(externalLayouts.begin(), externalLayouts.end(), bind2nd(gd::ExternalLayoutHasName(), name)) != externalLayouts.end() );
}
gd::ExternalLayout & Project::GetExternalLayout(const std::string & name)
{
    return *(*find_if(externalLayouts.begin(), externalLayouts.end(), bind2nd(gd::ExternalLayoutHasName(), name)));
}
const gd::ExternalLayout & Project::GetExternalLayout(const std::string & name) const
{
    return *(*find_if(externalLayouts.begin(), externalLayouts.end(), bind2nd(gd::ExternalLayoutHasName(), name)));
}
gd::ExternalLayout & Project::GetExternalLayout(unsigned int index)
{
    return *externalLayouts[index];
}
const gd::ExternalLayout & Project::GetExternalLayout (unsigned int index) const
{
    return *externalLayouts[index];
}
unsigned int Project::GetExternalLayoutPosition(const std::string & name) const
{
    for (unsigned int i = 0;i<externalLayouts.size();++i)
    {
        if ( externalLayouts[i]->GetName() == name ) return i;
    }
    return std::string::npos;
}

unsigned int Project::GetExternalLayoutsCount() const
{
    return externalLayouts.size();
}

gd::ExternalLayout & Project::InsertNewExternalLayout(const std::string & name, unsigned int position)
{
    boost::shared_ptr<gd::ExternalLayout> newExternalLayout = boost::shared_ptr<gd::ExternalLayout>(new gd::ExternalLayout);
    if (position<externalLayouts.size())
        externalLayouts.insert(externalLayouts.begin()+position, newExternalLayout);
    else
        externalLayouts.push_back(newExternalLayout);

    newExternalLayout->SetName(name);
    return *newExternalLayout;
}

void Project::InsertExternalLayout(const gd::ExternalLayout & layout, unsigned int position)
{
    boost::shared_ptr<gd::ExternalLayout> newLayout(new gd::ExternalLayout(layout));

    if (position<externalLayouts.size())
        externalLayouts.insert(externalLayouts.begin()+position, newLayout);
    else
        externalLayouts.push_back(newLayout);
}

void Project::RemoveExternalLayout(const std::string & name)
{
    std::vector< boost::shared_ptr<gd::ExternalLayout> >::iterator externalLayout = find_if(externalLayouts.begin(), externalLayouts.end(), bind2nd(gd::ExternalLayoutHasName(), name));
    if ( externalLayout == externalLayouts.end() ) return;

    externalLayouts.erase(externalLayout);
}

#if defined(GD_IDE_ONLY) && !defined(GD_NO_WX_GUI)
//Compatibility with GD2.x
class SpriteObjectsPositionUpdater : public gd::InitialInstanceFunctor
{
public:
    SpriteObjectsPositionUpdater(gd::Project & project_, gd::Layout & layout_) :
        project(project_),
        layout(layout_)
    {};
    virtual ~SpriteObjectsPositionUpdater() {};

    virtual void operator()(gd::InitialInstance * instancePtr)
    {
        gd::InitialInstance & instance = *instancePtr;
        gd::Object * object = NULL;
        if ( layout.HasObjectNamed(instance.GetObjectName()))
            object = &layout.GetObject(instance.GetObjectName());
        else if ( project.HasObjectNamed(instance.GetObjectName()))
            object = &project.GetObject(instance.GetObjectName());
        else return;

        if ( object->GetType() != "Sprite") return;
        if ( !instance.HasCustomSize() ) return;

        wxSetWorkingDirectory(wxFileName::FileName(project.GetProjectFile()).GetPath());
        object->LoadResources(project, layout);

        sf::Vector2f size = object->GetInitialInstanceDefaultSize(instance, project, layout);

        instance.SetX(instance.GetX() + size.x/2 - instance.GetCustomWidth()/2 );
        instance.SetY(instance.GetY() + size.y/2 - instance.GetCustomHeight()/2 );
    }

private:
    gd::Project & project;
    gd::Layout & layout;
};
//End of compatibility code
#endif

void Project::UnserializeFrom(const SerializerElement & element)
{
    //Checking version
    #if defined(GD_IDE_ONLY)
    std::string updateText;

    const SerializerElement & gdVersionElement = element.GetChild("gdVersion", 0, "GDVersion");
    GDMajorVersion = gdVersionElement.GetIntAttribute("major", GDMajorVersion, "Major");
    GDMinorVersion = gdVersionElement.GetIntAttribute("minor", GDMinorVersion, "Minor");
    int build = gdVersionElement.GetIntAttribute("build", 0, "Build");
    int revision = gdVersionElement.GetIntAttribute("revision", 0, "Revision");

    if ( GDMajorVersion > gd::VersionWrapper::Major() )
        gd::LogWarning( _( "The version of GDevelop used to create this game seems to be a new version.\nGDevelop may fail to open the game, or data may be missing.\nYou should check if a new version of GDevelop is available." ) );
    else
    {
        if ( (GDMajorVersion == gd::VersionWrapper::Major() && GDMinorVersion >  gd::VersionWrapper::Minor()) ||
             (GDMajorVersion == gd::VersionWrapper::Major() && GDMinorVersion == gd::VersionWrapper::Minor() && build >  gd::VersionWrapper::Build()) ||
             (GDMajorVersion == gd::VersionWrapper::Major() && GDMinorVersion == gd::VersionWrapper::Minor() && build == gd::VersionWrapper::Build() && revision > gd::VersionWrapper::Revision()) )
        {
            gd::LogWarning( _( "The version of GDevelop used to create this game seems to be greater.\nGDevelop may fail to open the game, or data may be missing.\nYou should check if a new version of GDevelop is available." ) );
        }
    }

    //Compatibility code
    if ( GDMajorVersion <= 1 )
    {
        gd::LogError(_("The game was saved with version of GDevelop which is too old. Please open and save the game with one of the first version of GDevelop 2. You will then be able to open your game with this GDevelop version."));
        return;
    }
    //End of Compatibility code
    #endif

    const SerializerElement & propElement = element.GetChild("properties", 0, "Info");
    SetName(propElement.GetChild("name", 0, "Nom").GetValue().GetString());
    SetDefaultWidth(propElement.GetChild("windowWidth", 0, "WindowW").GetValue().GetInt());
    SetDefaultHeight(propElement.GetChild("windowHeight", 0, "WindowH").GetValue().GetInt());
    SetMaximumFPS(propElement.GetChild("maxFPS", 0, "FPSmax").GetValue().GetInt());
    SetMinimumFPS(propElement.GetChild("minFPS", 0, "FPSmin").GetValue().GetInt());
    SetVerticalSyncActivatedByDefault(propElement.GetChild("verticalSync").GetValue().GetInt());
    #if defined(GD_IDE_ONLY)
    SetAuthor(propElement.GetChild("author", 0, "Auteur").GetValue().GetString());
    SetLastCompilationDirectory(propElement.GetChild("latestCompilationDirectory", 0, "LatestCompilationDirectory").GetValue().GetString());
    winExecutableFilename = propElement.GetStringAttribute("winExecutableFilename");
    winExecutableIconFile = propElement.GetStringAttribute("winExecutableIconFile");
    linuxExecutableFilename = propElement.GetStringAttribute("linuxExecutableFilename");
    macExecutableFilename = propElement.GetStringAttribute("macExecutableFilename");
    useExternalSourceFiles = propElement.GetBoolAttribute("useExternalSourceFiles");
    #endif

    const SerializerElement & extensionsElement = propElement.GetChild("extensions", 0, "Extensions");
    extensionsElement.ConsiderAsArrayOf("extension", "Extension");
    for(unsigned int i = 0;i<extensionsElement.GetChildrenCount();++i)
    {
        std::string extensionName = extensionsElement.GetChild(i).GetStringAttribute("name");
        if ( find(GetUsedExtensions().begin(), GetUsedExtensions().end(), extensionName ) == GetUsedExtensions().end() )
            GetUsedExtensions().push_back(extensionName);
    }

    #if defined(GD_IDE_ONLY)
    currentPlatform = NULL;
    std::string currentPlatformName = propElement.GetChild("currentPlatform").GetValue().GetString();
    //Compatibility code
    if ( VersionWrapper::IsOlderOrEqual(GDMajorVersion, GDMajorVersion, GDMinorVersion, 0, 3, 4, 73, 0) )
    {
        if (currentPlatformName == "Game Develop C++ platform") currentPlatformName = "GDevelop C++ platform";
        if (currentPlatformName == "Game Develop JS platform") currentPlatformName = "GDevelop JS platform";
    }
    //End of Compatibility code

    const SerializerElement & platformsElement = propElement.GetChild("platforms", 0, "Platforms");
    platformsElement.ConsiderAsArrayOf("platform", "Platform");
    for(unsigned int i = 0;i<platformsElement.GetChildrenCount();++i)
    {
        std::string name = platformsElement.GetChild(i).GetStringAttribute("name");
        //Compatibility code
        if ( VersionWrapper::IsOlderOrEqual(GDMajorVersion, GDMajorVersion, GDMinorVersion, 0, 3, 4, 73, 0) )
        {
            if (name == "Game Develop C++ platform") name = "GDevelop C++ platform";
            if (name == "Game Develop JS platform") name = "GDevelop JS platform";
        }
        //End of Compatibility code

        gd::Platform * platform = gd::PlatformManager::Get()->GetPlatform(name);

        if ( platform ) {
            AddPlatform(*platform);
            if ( platform->GetName() == currentPlatformName || currentPlatformName.empty() )
                currentPlatform = platform;
        }
        else {
            std::cout << "Platform \"" << name << "\" is unknown." << std::endl;
        }
    }

    //Compatibility code
    if ( platformsElement.GetChildrenCount() == 0 )
    {
        //Compatibility with GD2.x
        platforms.push_back(gd::PlatformManager::Get()->GetPlatform("GDevelop C++ platform"));
        currentPlatform = platforms.back();
    }
    //End of Compatibility code

    if (currentPlatform == NULL && !platforms.empty())
        currentPlatform = platforms.back();
    #endif

    //Compatibility code
    #if defined(GD_IDE_ONLY)
    if ( VersionWrapper::IsOlder(GDMajorVersion, 0, 0, 0, 3, 0, 0, 0) )
    {
        updateText += _("Sprite scaling has changed since GD 2: The resizing is made so that the origin point of the object won't move whatever the scale of the object.\n");
        updateText += _("You may have to slightly change the position of some objects if you have changed their size.\n\n");
        updateText += _("Thank you for your understanding.\n");

    }
    #endif
    //End of Compatibility code

    //Compatibility code
    #if defined(GD_IDE_ONLY)
    if ( VersionWrapper::IsOlderOrEqual(GDMajorVersion, GDMajorVersion, GDMinorVersion, build, 2,2,1,10822) )
    {
        if ( std::find(GetUsedExtensions().begin(), GetUsedExtensions().end(), "BuiltinExternalLayouts") == GetUsedExtensions().end() )
            GetUsedExtensions().push_back("BuiltinExternalLayouts");
    }
    #endif

    //Compatibility code
    #if defined(GD_IDE_ONLY)
    if ( VersionWrapper::IsOlderOrEqual(GDMajorVersion, GDMajorVersion, GDMinorVersion, build, 3,3,3,0) )
    {
        if ( std::find(GetUsedExtensions().begin(), GetUsedExtensions().end(), "AStarAutomatism") != GetUsedExtensions().end() )
        {
            GetUsedExtensions().erase( std::remove( GetUsedExtensions().begin(), GetUsedExtensions().end(), "AStarAutomatism" ), GetUsedExtensions().end() );
            GetUsedExtensions().push_back("PathfindingAutomatism");
            updateText += _("The project is using the pathfinding automatism. This automatism has been replaced by a new one:\n");
            updateText += _("You must add the new 'Pathfinding' automatism to the objects that need to be moved, and add the 'Pathfinding Obstacle' to the objects that must act as obstacles.");
        }
    }
    #endif

    #if defined(GD_IDE_ONLY)
    ObjectGroup::UnserializeFrom(GetObjectGroups(), element.GetChild("objectsGroups", 0, "ObjectGroups"));
    #endif
    resourcesManager.UnserializeFrom(element.GetChild( "resources", 0, "Resources" ));
    UnserializeObjectsFrom(*this, element.GetChild("objects", 0, "Objects"));
    GetVariables().UnserializeFrom(element.GetChild("variables", 0, "Variables"));

    const SerializerElement & layoutsElement = element.GetChild("layouts", 0, "Scenes");
    layoutsElement.ConsiderAsArrayOf("layout", "Scene");
    for(unsigned int i = 0;i<layoutsElement.GetChildrenCount();++i)
    {
        const SerializerElement & layoutElement = layoutsElement.GetChild(i);

        gd::Layout & layout = InsertNewLayout(layoutElement.GetStringAttribute("name", "", "nom"), -1);
        layout.UnserializeFrom(*this, layoutElement);

        //Compatibility code with GD 2.x
        #if defined(GD_IDE_ONLY) && !defined(GD_NO_WX_GUI)
        if ( GDMajorVersion <= 2 )
        {
            SpriteObjectsPositionUpdater updater(*this, layout);
            gd::InitialInstancesContainer & instances = layout.GetInitialInstances();
            instances.IterateOverInstances(updater);
        }
        #endif
        //End of compatibility code
    }

    #if defined(GD_IDE_ONLY)
    const SerializerElement & externalEventsElement = element.GetChild("externalEvents", 0, "ExternalEvents");
    externalEventsElement.ConsiderAsArrayOf("externalEvents", "ExternalEvents");
    for(unsigned int i = 0;i<externalEventsElement.GetChildrenCount();++i)
    {
        const SerializerElement & externalEventElement = externalEventsElement.GetChild(i);

        gd::ExternalEvents & externalEvents = InsertNewExternalEvents(externalEventElement.GetStringAttribute("name", "", "Name"),
            GetExternalEventsCount());
        externalEvents.UnserializeFrom(*this, externalEventElement);
    }
    #endif

    const SerializerElement & externalLayoutsElement = element.GetChild("externalLayouts", 0, "ExternalLayouts");
    externalLayoutsElement.ConsiderAsArrayOf("externalLayout", "ExternalLayout");
    for(unsigned int i = 0;i<externalLayoutsElement.GetChildrenCount();++i)
    {
        const SerializerElement & externalLayoutElement = externalLayoutsElement.GetChild(i);

        boost::shared_ptr<gd::ExternalLayout> newExternalLayout(new gd::ExternalLayout);
        newExternalLayout->UnserializeFrom(externalLayoutElement);
        externalLayouts.push_back(newExternalLayout);
    }

    #if defined(GD_IDE_ONLY)
    const SerializerElement & externalSourceFilesElement = element.GetChild("externalSourceFiles", 0, "ExternalSourceFiles");
    externalSourceFilesElement.ConsiderAsArrayOf("sourceFile", "SourceFile");
    for(unsigned int i = 0;i<externalSourceFilesElement.GetChildrenCount();++i)
    {
        const SerializerElement & sourceFileElement = externalSourceFilesElement.GetChild(i);

        boost::shared_ptr<gd::SourceFile> newSourceFile(new gd::SourceFile);
        newSourceFile->UnserializeFrom(sourceFileElement);
        externalSourceFiles.push_back(newSourceFile);
    }

    #if !defined(GD_NO_WX_GUI)
    if (!updateText.empty()) //TODO
    {
        ProjectUpdateDialog updateDialog(NULL, updateText);
        updateDialog.ShowModal();
    }

    dirty = false;
    #endif

    #endif
}

#if !defined(EMSCRIPTEN)
bool Project::LoadFromFile(const std::string & filename)
{
    //Load the XML document structure
    TiXmlDocument doc;
    if ( !doc.LoadFile(filename.c_str()) )
    {
        std::string errorTinyXmlDesc = doc.ErrorDesc();
        std::string error = gd::ToString(_( "Error while loading :" )) + "\n" + errorTinyXmlDesc + "\n\n" +_("Make sure the file exists and that you have the right to open the file.");

        gd::LogError( error );
        return false;
    }

    #if defined(GD_IDE_ONLY)
    SetProjectFile(filename);
    dirty = false;
    #endif

    TiXmlHandle hdl( &doc );
    gd::SerializerElement rootElement;

    TiXmlElement * rootXmlElement = hdl.FirstChildElement("project").ToElement();
    //Compatibility with GD <= 3.3
    if (!rootXmlElement) rootXmlElement = hdl.FirstChildElement("Project").ToElement();
    if (!rootXmlElement) rootXmlElement = hdl.FirstChildElement("Game").ToElement();
    //End of compatibility code
    gd::Serializer::FromXML(rootElement, rootXmlElement);

    //Unserialize the whole project
    UnserializeFrom(rootElement);

    return true;
}

#if defined(GD_IDE_ONLY)
bool Project::LoadFromJSONFile(const std::string & filename)
{
    std::ifstream ifs(filename.c_str());
    if (!ifs.is_open())
    {
        std::string error = gd::ToString(_( "Unable to open the file")) +_("Make sure the file exists and that you have the right to open the file.");
        gd::LogError( error );
        return false;
    }

    SetProjectFile(filename);
    dirty = false;

    std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    gd::SerializerElement rootElement = gd::Serializer::FromJSON(str);
    UnserializeFrom(rootElement);

    return true;
}
#endif
#endif

#if defined(GD_IDE_ONLY)
void Project::SerializeTo(SerializerElement & element) const
{
    SerializerElement & versionElement = element.AddChild("gdVersion");
    versionElement.SetAttribute("major", ToString( gd::VersionWrapper::Major() ) );
    versionElement.SetAttribute("minor", ToString( gd::VersionWrapper::Minor() ) );
    versionElement.SetAttribute("build", ToString( gd::VersionWrapper::Build() ) );
    versionElement.SetAttribute("revision", ToString( gd::VersionWrapper::Revision() ) );

    SerializerElement & propElement = element.AddChild("properties");
    propElement.AddChild("name").SetValue(GetName());
    propElement.AddChild("author").SetValue(GetAuthor());
    propElement.AddChild("windowWidth").SetValue(GetMainWindowDefaultWidth());
    propElement.AddChild("windowHeight").SetValue(GetMainWindowDefaultHeight());
    propElement.AddChild("latestCompilationDirectory").SetValue(GetLastCompilationDirectory());
    propElement.AddChild("maxFPS").SetValue(GetMaximumFPS());
    propElement.AddChild("minFPS").SetValue(GetMinimumFPS());
    propElement.AddChild("verticalSync").SetValue(IsVerticalSynchronizationEnabledByDefault());
    propElement.SetAttribute("winExecutableFilename", winExecutableFilename);
    propElement.SetAttribute("winExecutableIconFile", winExecutableIconFile);
    propElement.SetAttribute("linuxExecutableFilename", linuxExecutableFilename);
    propElement.SetAttribute("macExecutableFilename", macExecutableFilename);
    propElement.SetAttribute("useExternalSourceFiles", useExternalSourceFiles);

    SerializerElement & extensionsElement = propElement.AddChild("extensions");
    extensionsElement.ConsiderAsArrayOf("extension");
    for (unsigned int i =0;i<GetUsedExtensions().size();++i)
        extensionsElement.AddChild("extension").SetAttribute("name", GetUsedExtensions()[i]);

    SerializerElement & platformsElement = propElement.AddChild("platforms");
    platformsElement.ConsiderAsArrayOf("platform");
    for (unsigned int i =0;i<platforms.size();++i) {
        if (platforms[i] == NULL) {
            std::cout << "ERROR: The project has a platform which is NULL.";
            continue;
        }

        platformsElement.AddChild("platform").SetAttribute("name", platforms[i]->GetName());
    }
    if (currentPlatform != NULL)
        propElement.AddChild("currentPlatform").SetValue(currentPlatform->GetName());
    else
        std::cout << "ERROR: The project current platform is NULL.";

    resourcesManager.SerializeTo(element.AddChild("resources"));
    SerializeObjectsTo(element.AddChild("objects"));
    ObjectGroup::SerializeTo(GetObjectGroups(), element.AddChild("objectsGroups"));
    GetVariables().SerializeTo(element.AddChild("variables"));

    element.SetAttribute("firstLayout", firstLayout);
    gd::SerializerElement & layoutsElement = element.AddChild("layouts");
    layoutsElement.ConsiderAsArrayOf("layout");
    for ( unsigned int i = 0;i < GetLayoutsCount();i++ )
        GetLayout(i).SerializeTo(layoutsElement.AddChild("layout"));

    SerializerElement & externalEventsElement = element.AddChild("externalEvents");
    externalEventsElement.ConsiderAsArrayOf("externalEvents");
    for (unsigned int i =0;i<GetExternalEventsCount();++i)
        GetExternalEvents(i).SerializeTo(externalEventsElement.AddChild("externalEvents"));

    SerializerElement & externalLayoutsElement = element.AddChild("externalLayouts");
    externalLayoutsElement.ConsiderAsArrayOf("externalLayout");
    for (unsigned int i =0;i<externalLayouts.size();++i)
        externalLayouts[i]->SerializeTo(externalLayoutsElement.AddChild("externalLayout"));

    SerializerElement & externalSourceFilesElement = element.AddChild("externalSourceFiles");
    externalSourceFilesElement.ConsiderAsArrayOf("sourceFile");
    for (unsigned int i =0;i<externalSourceFiles.size();++i)
        externalSourceFiles[i]->SerializeTo(externalSourceFilesElement.AddChild("sourceFile"));

    #if defined(GD_IDE_ONLY)
    dirty = false;
    #endif
}

#if !defined(EMSCRIPTEN)
bool Project::SaveToFile(const std::string & filename)
{
    //Serialize the whole project
    gd::SerializerElement rootElement;
    SerializeTo(rootElement);

    //Create the XML document structure...
    TiXmlDocument doc;
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "ISO-8859-1", "" );
    doc.LinkEndChild( decl );

    TiXmlElement * root = new TiXmlElement( "project" );
    doc.LinkEndChild( root );
    gd::Serializer::ToXML(rootElement, root); //...and put the serialized project in it.

    //Write XML to file
    if ( !doc.SaveFile( filename.c_str() ) )
    {
        gd::LogError( _( "Unable to save file ")+filename+_("!\nCheck that the drive has enough free space, is not write-protected and that you have read/write permissions." ) );
        return false;
    }

    return true;
}

bool Project::SaveToJSONFile(const std::string & filename)
{
    //Serialize the whole project
    gd::SerializerElement rootElement;
    SerializeTo(rootElement);

    //Write JSON to file
    std::string str = gd::Serializer::ToJSON(rootElement);
    ofstream ofs(filename.c_str());
    if (!ofs.is_open())
    {
        gd::LogError( _( "Unable to save file ")+filename+_("!\nCheck that the drive has enough free space, is not write-protected and that you have read/write permissions." ) );
        return false;
    }

    ofs << str;
    ofs.close();
    return true;
}
#endif

bool Project::ValidateObjectName(const std::string & name)
{
    std::string allowedCharacter = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
    return !(name.find_first_not_of(allowedCharacter) != std::string::npos);
}

std::string Project::GetBadObjectNameWarning()
{
    return gd::ToString(_("Please use only letters, digits\nand underscores ( _ )."));
}

void Project::ExposeResources(gd::ArbitraryResourceWorker & worker)
{
    //Add project resources
    std::vector<std::string> resources = GetResourcesManager().GetAllResourcesList();
    for ( unsigned int i = 0;i < resources.size() ;i++ )
    {
        if ( GetResourcesManager().GetResource(resources[i]).UseFile() )
            worker.ExposeResource(GetResourcesManager().GetResource(resources[i]));
    }
    #if !defined(GD_NO_WX_GUI)
    wxSafeYield();
    #endif

    //Add layouts resources
    for ( unsigned int s = 0;s < GetLayoutsCount();s++ )
    {
        for (unsigned int j = 0;j<GetLayout(s).GetObjectsCount();++j) //Add objects resources
        	GetLayout(s).GetObject(j).ExposeResources(worker);

        LaunchResourceWorkerOnEvents(*this, GetLayout(s).GetEvents(), worker);
    }
    //Add external events resources
    for ( unsigned int s = 0;s < GetExternalEventsCount();s++ )
    {
        LaunchResourceWorkerOnEvents(*this, GetExternalEvents(s).GetEvents(), worker);
    }
    #if !defined(GD_NO_WX_GUI)
    wxSafeYield();
    #endif

    //Add global objects resources
    for (unsigned int j = 0;j<GetObjectsCount();++j)
        GetObject(j).ExposeResources(worker);

    #if !defined(GD_NO_WX_GUI)
    wxSafeYield();
    #endif
}

bool Project::HasSourceFile(std::string name, std::string language) const
{
    vector< boost::shared_ptr<SourceFile> >::const_iterator sourceFile =
        find_if(externalSourceFiles.begin(), externalSourceFiles.end(),
        bind2nd(gd::ExternalSourceFileHasName(), name));

    if (sourceFile == externalSourceFiles.end())
        return false;

    return language.empty() || (*sourceFile)->GetLanguage() == language;
}

gd::SourceFile & Project::GetSourceFile(const std::string & name)
{
    return *(*find_if(externalSourceFiles.begin(), externalSourceFiles.end(), bind2nd(gd::ExternalSourceFileHasName(), name)));
}

const gd::SourceFile & Project::GetSourceFile(const std::string & name) const
{
    return *(*find_if(externalSourceFiles.begin(), externalSourceFiles.end(), bind2nd(gd::ExternalSourceFileHasName(), name)));
}

void Project::RemoveSourceFile(const std::string & name)
{
    std::vector< boost::shared_ptr<gd::SourceFile> >::iterator sourceFile =
        find_if(externalSourceFiles.begin(), externalSourceFiles.end(), bind2nd(gd::ExternalSourceFileHasName(), name));
    if ( sourceFile == externalSourceFiles.end() ) return;

    externalSourceFiles.erase(sourceFile);
}

gd::SourceFile & Project::InsertNewSourceFile(const std::string & name, const std::string & language, unsigned int position)
{
    if (HasSourceFile(name, language))
        return GetSourceFile(name);

    boost::shared_ptr<SourceFile> newSourceFile(new SourceFile);
    newSourceFile->SetLanguage(language);
    newSourceFile->SetFileName(name);

    if (position<externalSourceFiles.size())
        externalSourceFiles.insert(externalSourceFiles.begin()+position, newSourceFile);
    else
        externalSourceFiles.push_back(newSourceFile);

    return *newSourceFile;
}

#if !defined(GD_NO_WX_GUI)
void Project::PopulatePropertyGrid(wxPropertyGrid * grid)
{
    grid->Append( new wxPropertyCategory(_("Properties")) );
    grid->Append( new wxStringProperty(_("Name of the project"), wxPG_LABEL, GetName()) );
    grid->Append( new wxStringProperty(_("Author"), wxPG_LABEL, GetAuthor()) );
    grid->Append( new wxStringProperty(_("Globals variables"), wxPG_LABEL, _("Click to edit...")) );
    grid->Append( new wxStringProperty(_("Extensions"), wxPG_LABEL, _("Click to edit...")) );
    grid->Append( new wxPropertyCategory(_("Window")) );
    grid->Append( new wxUIntProperty(_("Width"), wxPG_LABEL, GetMainWindowDefaultWidth()) );
    grid->Append( new wxUIntProperty(_("Height"), wxPG_LABEL, GetMainWindowDefaultHeight()) );
    grid->Append( new wxBoolProperty(_("Vertical Synchronization"), wxPG_LABEL, IsVerticalSynchronizationEnabledByDefault()) );
    grid->Append( new wxBoolProperty(_("Limit the framerate"), wxPG_LABEL, GetMaximumFPS() != -1) );
    grid->Append( new wxIntProperty(_("Maximum FPS"), wxPG_LABEL, GetMaximumFPS()) );
    grid->Append( new wxUIntProperty(_("Minimum FPS"), wxPG_LABEL, GetMinimumFPS()) );

    grid->SetPropertyCell(wxString(_("Globals variables")), 1, _("Click to edit..."), wxNullBitmap, wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT ));
    grid->SetPropertyReadOnly(wxString(_("Globals variables")));
    grid->SetPropertyCell(wxString(_("Extensions")), 1, _("Click to edit..."), wxNullBitmap, wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT ));
    grid->SetPropertyReadOnly(wxString(_("Extensions")));

    if ( GetMaximumFPS() == -1 )
    {
        grid->GetProperty(_("Maximum FPS"))->Enable(false);
        grid->GetProperty(_("Maximum FPS"))->SetValue("");
    }
    else
        grid->GetProperty(_("Maximum FPS"))->Enable(true);

    grid->Append( new wxPropertyCategory(_("Generation")) );
    grid->Append( new wxStringProperty(_("Windows executable name"), wxPG_LABEL, winExecutableFilename) );
    grid->Append( new wxImageFileProperty(_("Windows executable icon"), wxPG_LABEL, winExecutableIconFile) );
    grid->Append( new wxStringProperty(_("Linux executable name"), wxPG_LABEL, linuxExecutableFilename) );
    grid->Append( new wxStringProperty(_("Mac OS executable name"), wxPG_LABEL, macExecutableFilename) );

    grid->Append( new wxPropertyCategory(_("C++ features")) );
    grid->Append( new wxBoolProperty(_("Activate the use of C++/JS source files"), wxPG_LABEL, useExternalSourceFiles) );
}

void Project::UpdateFromPropertyGrid(wxPropertyGrid * grid)
{
    if ( grid->GetProperty(_("Name of the project")) != NULL)
        SetName(gd::ToString(grid->GetProperty(_("Name of the project"))->GetValueAsString()));
    if ( grid->GetProperty(_("Author")) != NULL)
        SetAuthor(gd::ToString(grid->GetProperty(_("Author"))->GetValueAsString()));
    if ( grid->GetProperty(_("Width")) != NULL)
        SetDefaultWidth(grid->GetProperty(_("Width"))->GetValue().GetInteger());
    if ( grid->GetProperty(_("Height")) != NULL)
        SetDefaultHeight(grid->GetProperty(_("Height"))->GetValue().GetInteger());
    if ( grid->GetProperty(_("Vertical Synchronization")) != NULL)
        SetVerticalSyncActivatedByDefault(grid->GetProperty(_("Vertical Synchronization"))->GetValue().GetBool());
    if ( grid->GetProperty(_("Limit the framerate")) != NULL && !grid->GetProperty(_("Limit the framerate"))->GetValue().GetBool())
        SetMaximumFPS(-1);
    else if ( grid->GetProperty(_("Maximum FPS")) != NULL)
        SetMaximumFPS(grid->GetProperty(_("Maximum FPS"))->GetValue().GetInteger());
    if ( grid->GetProperty(_("Minimum FPS")) != NULL)
        SetMinimumFPS(grid->GetProperty(_("Minimum FPS"))->GetValue().GetInteger());

    if ( grid->GetProperty(_("Windows executable name")) != NULL)
        winExecutableFilename = gd::ToString(grid->GetProperty(_("Windows executable name"))->GetValueAsString());
    if ( grid->GetProperty(_("Windows executable icon")) != NULL)
        winExecutableIconFile = gd::ToString(grid->GetProperty(_("Windows executable icon"))->GetValueAsString());
    if ( grid->GetProperty(_("Linux executable name")) != NULL)
        linuxExecutableFilename = gd::ToString(grid->GetProperty(_("Linux executable name"))->GetValueAsString());
    if ( grid->GetProperty(_("Mac OS executable name")) != NULL)
        macExecutableFilename = gd::ToString(grid->GetProperty(_("Mac OS executable name"))->GetValueAsString());
    if ( grid->GetProperty(_("Activate the use of C++/JS source files")) != NULL)
        useExternalSourceFiles =grid->GetProperty(_("Activate the use of C++/JS source files"))->GetValue().GetBool();
}

void Project::OnChangeInPropertyGrid(wxPropertyGrid * grid, wxPropertyGridEvent & event)
{
    if (event.GetPropertyName() == _("Limit the framerate") )
        grid->EnableProperty(wxString(_("Maximum FPS")), grid->GetProperty(_("Limit the framerate"))->GetValue().GetBool());

    UpdateFromPropertyGrid(grid);
}

void Project::OnSelectionInPropertyGrid(wxPropertyGrid * grid, wxPropertyGridEvent & event)
{
    if ( event.GetColumn() == 1) //Manage button-like properties
    {
        if ( event.GetPropertyName() == _("Extensions") )
        {
            gd::ProjectExtensionsDialog dialog(NULL, *this);
            dialog.ShowModal();
        }
        else if ( event.GetPropertyName() == _("Globals variables") )
        {
            gd::ChooseVariableDialog dialog(NULL, GetVariables(), /*editingOnly=*/true);
            dialog.SetAssociatedProject(this);
            dialog.ShowModal();
        }
    }
}
#endif
#endif

Project::Project(const Project & other)
{
    Init(other);
}

Project& Project::operator=(const Project & other)
{
    if ( this != &other )
        Init(other);

    return *this;
}

void Project::Init(const gd::Project & game)
{
    //Some properties
    name = game.name;
    windowWidth = game.windowWidth;
    windowHeight = game.windowHeight;
    maxFPS = game.maxFPS;
    minFPS = game.minFPS;
    verticalSync = game.verticalSync;

    #if defined(GD_IDE_ONLY)
    author = game.author;
    latestCompilationDirectory = game.latestCompilationDirectory;
    objectGroups = game.objectGroups;

    GDMajorVersion = game.GDMajorVersion;
    GDMinorVersion = game.GDMinorVersion;

    currentPlatform = game.currentPlatform;
    #endif
    extensionsUsed = game.extensionsUsed;
    platforms = game.platforms;

    //Resources
    resourcesManager = game.resourcesManager;
    #if !defined(GD_NO_WX_GUI)
    imageManager = boost::shared_ptr<ImageManager>(new ImageManager(*game.imageManager));
    imageManager->SetGame(this);
    #endif

    GetObjects().clear();
    for (unsigned int i =0;i<game.GetObjects().size();++i)
    	GetObjects().push_back( boost::shared_ptr<gd::Object>(game.GetObjects()[i]->Clone()) );

    scenes.clear();
    for (unsigned int i =0;i<game.scenes.size();++i)
    	scenes.push_back( boost::shared_ptr<gd::Layout>(new gd::Layout(*game.scenes[i])) );

    #if defined(GD_IDE_ONLY)
    externalEvents.clear();
    for (unsigned int i =0;i<game.externalEvents.size();++i)
    	externalEvents.push_back( boost::shared_ptr<gd::ExternalEvents>(new gd::ExternalEvents(*game.externalEvents[i])) );
    #endif

    externalLayouts.clear();
    for (unsigned int i =0;i<game.externalLayouts.size();++i)
    	externalLayouts.push_back( boost::shared_ptr<gd::ExternalLayout>(new gd::ExternalLayout(*game.externalLayouts[i])) );

    #if defined(GD_IDE_ONLY)
    useExternalSourceFiles = game.useExternalSourceFiles;

    externalSourceFiles.clear();
    for (unsigned int i =0;i<game.externalSourceFiles.size();++i)
    	externalSourceFiles.push_back( boost::shared_ptr<gd::SourceFile>(new gd::SourceFile(*game.externalSourceFiles[i])) );
    #endif

    variables = game.GetVariables();

    #if defined(GD_IDE_ONLY)
    gameFile = game.GetProjectFile();
    imagesChanged = game.imagesChanged;

    winExecutableFilename = game.winExecutableFilename;
    winExecutableIconFile = game.winExecutableIconFile;
    linuxExecutableFilename = game.linuxExecutableFilename;
    macExecutableFilename = game.macExecutableFilename;
    #endif
}

}
