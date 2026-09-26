# Fizzle Tutorial System

## Here's everything you need to create in the editor to get the plugin functional:

### Required Assets

  1. Tutorial Data Asset Right-click Content Browser -> Miscellaneous -> Data Asset  -> pick TutorialDataAsset.

  **Fill in:**
  - TutorialName - e.g. "Movement Tutorial"
  - Steps array - add one entry per step, each needs:
    - StepTag - a Gameplay Tag (e.g. Tutorial.Step.Move, Tutorial.Step.Jump)
    - Title / Description - what the UI shows
    - CompletionType - OverlapVolume, CodeTriggered, TimedAuto, or GameplayEvent
    - HintText  - optional
    - HintDelaySeconds - optional
    - bMandatory - if it can be skipped

  **You need one of these per tutorial sequence.**

 2. Gameplay Tags In Project Settings -> Gameplay Tags, add a tag for each step that matches the StepTag you set above. Example hierarchy:

  Tutorial
    Tutorial.Step
      Tutorial.Step.Move
      Tutorial.Step.Jump
      Tutorial.Step.Shoot

  3. Tutorial UI Widget (UMG) Create a WBP_TutorialHUD Widget Blueprint. This is the popup that shows step info to the player.

  Bind it to the UTutorialComponent events on your character BP:
  - On Step Activated -> show the widget, populate Title/Description text
  - On Step Completed -> play a "check" animation or hide
  - On Hint Ready -> reveal the hint text
  - On Tutorial Completed -> hide the widget entirely

  **No specific structure is required - the plugin just fires events, your widget decides how to present them.**


  4. Character Blueprint update Open your existing character BP (whatever extends ABCharacter):

  - Add UTutorialComponent as a component
  - The BP_OnStepActivated, BP_OnStepCompleted, etc. events will now be available to override directly in that BP


  5. Tutorial Trigger Volumes (Level Actors) For any step with CompletionType = OverlapVolume:

  - In the level, use Place Actors panel -> search "Tutorial Trigger Volume"  -> drag into the level
  - Resize/position the brush to cover the area
  - Set StepTag on the volume to match the corresponding step in your data asset
  - Optionally set RequiredActorClass to your character class


  6. Start the Tutorial (Game Mode or Level Blueprint) Somewhere at game start, call Start Tutorial on the subsystem:

  Get Tutorial Subsystem (self) -> Start Tutorial (your DA_TutorialAsset)

  This is typically done in your Game Mode BeginPlay, or from the Level Blueprint after any intro sequence finishes.


### Optional but Recommended

| Asset                            | Purpose                                                                              | 
|----------------------------------|--------------------------------------------------------------------------------------|
|DA_Tutorial_* naming convention  | One data asset per tutorial phase (movement, combat, etc.)                            |
|WBP_TutorialStep as a sub-widget | A reusable step card with Title, Description, Icon, progress bar                      |
|Save Game integration            | Store which tutorials have been completed in BSaveGame so they don't replay on reload |
|Gameplay Tag table in .ini       | Add +GameplayTagList entries to DefaultGameplayTags.ini to source-control your tags   |
