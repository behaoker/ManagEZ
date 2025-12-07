/*
  Minimal VST3 Checklist Plugin - Editor Implementation

  Bare-bones UI with no fancy features.
*/

#include "PluginEditor.h"

SimpleChecklistEditor::SimpleChecklistEditor(SimpleChecklistProcessor &p)
    : AudioProcessorEditor(&p), processor(p) {
  setSize(400, 500);

  // Set window icon
  auto iconData = BinaryData::icon_png;
  auto iconSize = BinaryData::icon_pngSize;
  if (iconData != nullptr && iconSize > 0) {
    auto iconImage = juce::ImageCache::getFromMemory(iconData, iconSize);
    if (iconImage.isValid()) {
      setIcon(iconImage);
    }
  }

  // Input box with dark styling
  addAndMakeVisible(inputBox);
  inputBox.setMultiLine(false);
  inputBox.setReturnKeyStartsNewLine(false);
  inputBox.setTextToShowWhenEmpty("Add a task...", juce::Colour(0xff888888));
  inputBox.setColour(juce::TextEditor::backgroundColourId,
                     juce::Colour(0xff2d2d2d));
  inputBox.setColour(juce::TextEditor::textColourId, juce::Colours::white);
  inputBox.setColour(juce::TextEditor::outlineColourId,
                     juce::Colour(0xff404040));
  inputBox.addListener(this);

  // Add button with styling
  addAndMakeVisible(addButton);
  addButton.setButtonText("Add");
  addButton.setColour(juce::TextButton::buttonColourId,
                      juce::Colour(0xff0078d4));
  addButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
  addButton.addListener(this);

  // Task viewport with dark background
  addAndMakeVisible(taskViewport);
  taskViewport.setViewedComponent(&taskContainer, false);
  taskViewport.setScrollBarsShown(true, false);

  // Listen for task changes
  processor.addListener(this);

  // Build initial task list
  rebuildTaskList();
}

SimpleChecklistEditor::~SimpleChecklistEditor() {
  processor.removeListener(this);
}

void SimpleChecklistEditor::paint(juce::Graphics &g) {
  // Dark mode background
  g.fillAll(juce::Colour(0xff1e1e1e)); // Dark gray background

  // Title with white text
  g.setColour(juce::Colours::white);
  g.setFont(juce::Font(20.0f, juce::Font::bold));
  g.drawText("ManagEZ", 0, 10, getWidth(), 30, juce::Justification::centred);
}

void SimpleChecklistEditor::resized() {
  auto area = getLocalBounds().reduced(10);

  area.removeFromTop(40); // Title space

  // Input row
  auto inputRow = area.removeFromTop(30);
  addButton.setBounds(inputRow.removeFromRight(60));
  inputRow.removeFromRight(5);
  inputBox.setBounds(inputRow);

  area.removeFromTop(10);

  // Task list
  taskViewport.setBounds(area);
}

void SimpleChecklistEditor::buttonClicked(juce::Button *button) {
  if (button == &addButton) {
    addTaskFromInput();
    return;
  }

  // Check if it's a delete button
  for (int i = 0; i < deleteButtons.size(); ++i) {
    if (button == deleteButtons[i]) {
      processor.removeTask(i);
      return;
    }
  }

  // Check if it's a checkbox
  for (int i = 0; i < checkboxes.size(); ++i) {
    if (button == checkboxes[i]) {
      processor.toggleTask(i);
      return;
    }
  }
}

void SimpleChecklistEditor::textEditorReturnKeyPressed(juce::TextEditor &) {
  addTaskFromInput();
}

void SimpleChecklistEditor::addTaskFromInput() {
  juce::String text = inputBox.getText().trim();
  if (text.isNotEmpty()) {
    processor.addTask(text);
    inputBox.clear();
  }
}

void SimpleChecklistEditor::tasksChanged() { rebuildTaskList(); }

void SimpleChecklistEditor::rebuildTaskList() {
  checkboxes.clear();
  labels.clear();
  deleteButtons.clear();

  const auto &tasks = processor.getTasks();
  int yPos = 0;

  for (int i = 0; i < (int)tasks.size(); ++i) {
    const auto &task = tasks[i];

    // Checkbox
    auto *checkbox = new juce::ToggleButton();
    checkbox->setToggleState(task.completed, juce::dontSendNotification);
    checkbox->addListener(this);
    checkbox->setBounds(10, yPos, 30, 30);
    taskContainer.addAndMakeVisible(checkbox);
    checkboxes.add(checkbox);

    // Label with white text for dark mode
    auto *label = new juce::Label();
    label->setText(task.text, juce::dontSendNotification);
    label->setColour(juce::Label::textColourId, juce::Colours::white);
    label->setColour(juce::Label::backgroundColourId,
                     juce::Colours::transparentBlack);
    label->setBounds(45, yPos, 280, 30);
    taskContainer.addAndMakeVisible(label);
    labels.add(label);

    // Delete button with red styling
    auto *delBtn = new juce::TextButton("X");
    delBtn->setColour(juce::TextButton::buttonColourId,
                      juce::Colour(0xff8b0000));
    delBtn->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    delBtn->addListener(this);
    delBtn->setBounds(330, yPos, 40, 30);
    taskContainer.addAndMakeVisible(delBtn);
    deleteButtons.add(delBtn);

    yPos += 35;
  }

  taskContainer.setSize(380, yPos);
}
