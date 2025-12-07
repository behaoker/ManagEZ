/*
  Minimal VST3 Checklist Plugin - Editor Implementation

  Bare-bones UI with no fancy features.
*/

#include "PluginEditor.h"

SimpleChecklistEditor::SimpleChecklistEditor(SimpleChecklistProcessor &p)
    : AudioProcessorEditor(&p), processor(p) {
  setSize(400, 500);

  // Input box
  addAndMakeVisible(inputBox);
  inputBox.setMultiLine(false);
  inputBox.setReturnKeyStartsNewLine(false);
  inputBox.setTextToShowWhenEmpty("Add a task...", juce::Colours::grey);
  inputBox.addListener(this);

  // Add button
  addAndMakeVisible(addButton);
  addButton.setButtonText("Add");
  addButton.addListener(this);

  // Task viewport
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
  g.fillAll(juce::Colours::white);

  // Simple title
  g.setColour(juce::Colours::black);
  g.setFont(20.0f);
  g.drawText("Simple Checklist", 0, 10, getWidth(), 30,
             juce::Justification::centred);
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

    // Label
    auto *label = new juce::Label();
    label->setText(task.text, juce::dontSendNotification);
    label->setBounds(45, yPos, 280, 30);
    taskContainer.addAndMakeVisible(label);
    labels.add(label);

    // Delete button
    auto *delBtn = new juce::TextButton("X");
    delBtn->addListener(this);
    delBtn->setBounds(330, yPos, 40, 30);
    taskContainer.addAndMakeVisible(delBtn);
    deleteButtons.add(delBtn);

    yPos += 35;
  }

  taskContainer.setSize(380, yPos);
}
