/*
  Minimal VST3 Checklist Plugin - Editor Header

  Super simple UI with just the basics.
*/

#pragma once

#include "PluginProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>

class SimpleChecklistEditor : public juce::AudioProcessorEditor,
                              private juce::Button::Listener,
                              private juce::TextEditor::Listener,
                              private SimpleChecklistProcessor::Listener {
public:
  SimpleChecklistEditor(SimpleChecklistProcessor &);
  ~SimpleChecklistEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

private:
  // Callbacks
  void buttonClicked(juce::Button *) override;
  void textEditorReturnKeyPressed(juce::TextEditor &) override;
  void tasksChanged() override;

  void addTaskFromInput();
  void rebuildTaskList();

  SimpleChecklistProcessor &processor;

  // UI Components - MINIMAL!
  juce::TextEditor inputBox;
  juce::TextButton addButton;
  juce::Viewport taskViewport;
  juce::Component taskContainer;

  // Task item components
  juce::OwnedArray<juce::ToggleButton> checkboxes;
  juce::OwnedArray<juce::Label> labels;
  juce::OwnedArray<juce::TextButton> deleteButtons;

  // Logo
  juce::Image logoImage;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleChecklistEditor)
};
