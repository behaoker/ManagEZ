/*
  ManagEZ v1.0 - Editor Header
*/

#pragma once

#include "PluginProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>

class SimpleChecklistEditor : public juce::AudioProcessorEditor,
                              private juce::Button::Listener,
                              private juce::TextEditor::Listener,
                              private juce::ComboBox::Listener,
                              private SimpleChecklistProcessor::Listener {
public:
  explicit SimpleChecklistEditor(SimpleChecklistProcessor &);
  ~SimpleChecklistEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

private:
  // Button callbacks
  void buttonClicked(juce::Button *button) override;
  void textEditorReturnKeyPressed(juce::TextEditor &) override;
  void comboBoxChanged(juce::ComboBox *comboBox) override;

  // Mouse events for editing and context menu
  void mouseDoubleClick(const juce::MouseEvent &event) override;
  void mouseDown(const juce::MouseEvent &event) override;

  // Task management
  void addTaskFromInput();
  void tasksChanged() override;
  void rebuildTaskList();
  void showContextMenu(int taskIndex, juce::Point<int> position);
  int getTaskIndexAtPosition(int y);

  // Helper functions
  juce::Colour getPriorityColour(Priority priority);
  juce::String getCategoryName(Category category);
  juce::String getCategorySymbol(Category category);

  SimpleChecklistProcessor &processor;

  // Top controls
  juce::TextEditor searchBox;
  juce::ComboBox templateSelector;
  juce::Label progressLabel;
  juce::Image logoImage;

  // Input controls
  juce::TextEditor inputBox;
  juce::TextButton addButton;

  // Task list view
  juce::Viewport taskViewport;
  juce::Component taskContainer;

  // Task item components
  juce::OwnedArray<juce::ToggleButton> checkboxes;
  juce::OwnedArray<juce::Label> labels;
  juce::OwnedArray<juce::TextButton> deleteButtons;
  juce::OwnedArray<juce::Label> priorityLabels;
  juce::OwnedArray<juce::Label> categoryLabels;

  // Editing state
  juce::TextEditor *editingEditor;
  int editingTaskIndex;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleChecklistEditor)
};
