#include "PluginScanner.h"

PluginScanner::PluginScanner()
{
    formatManager.addDefaultFormats();
    loadPluginList();
}

void PluginScanner::scanForPlugins()
{
    if (scanning)
        return;
    
    scanning = true;
    
    juce::FileSearchPath searchPath;
    
    // Add default VST3 paths based on platform
   #if JUCE_MAC
    searchPath.add (juce::File ("~/Library/Audio/Plug-Ins/VST3"));
    searchPath.add (juce::File ("/Library/Audio/Plug-Ins/VST3"));
   #elif JUCE_WINDOWS
    searchPath.add (juce::File ("C:\\Program Files\\Common Files\\VST3"));
   #elif JUCE_LINUX
    searchPath.add (juce::File ("~/.vst3"));
    searchPath.add (juce::File ("/usr/lib/vst3"));
   #endif
    
    // Scan each format
    for (int i = 0; i < formatManager.getNumFormats(); ++i)
    {
        auto* format = formatManager.getFormat (i);
        
        juce::PluginDirectoryScanner scanner (knownPlugins, *format, searchPath,
                                             true, juce::File());
        
        juce::String pluginName;
        while (scanner.scanNextFile (false, pluginName))
        {
            // Continue scanning...
        }
    }
    
    savePluginList();
    scanning = false;
}

std::unique_ptr<juce::AudioPluginInstance> PluginScanner::createPlugin (int index)
{
    auto types = knownPlugins.getTypes();
    
    if (!juce::isPositiveAndBelow (index, types.size()))
        return nullptr;
    
    juce::String errorMessage;
    
    return formatManager.createPluginInstance (types[index], 44100.0, 512, errorMessage);
}

juce::File PluginScanner::getPluginListFile() const
{
    return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
               .getChildFile ("BrokenArrowStudio")
               .getChildFile ("PluginList.xml");
}

void PluginScanner::savePluginList()
{
    auto file = getPluginListFile();
    file.getParentDirectory().createDirectory();
    
    if (auto xml = knownPlugins.createXml())
        xml->writeTo (file);
}

void PluginScanner::loadPluginList()
{
    auto file = getPluginListFile();
    
    if (file.existsAsFile())
    {
        if (auto xml = juce::XmlDocument::parse (file))
            knownPlugins.recreateFromXml (*xml);
    }
}
