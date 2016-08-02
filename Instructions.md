# Instructions

todo - video 

## Introduction 

Like many bespoke tools, BPainT has a bit of a learning curve and has some rough edges (and sadly even some bugs). This tool was made for artists who had familiarity with similar tools or who were given hands on training in how to use. I will look into making some YouTube videos to serve this purpose. In the mean time hopefully a few screen shots and a brief description of some items will help.

### Some terms

* Showcase - a collection of one or more animations (bpt files are showcase containers).
* Animation - a collection of layer layers with animation frames.
* Annotation (or Text Annotation) - an editable text blob for scripting or notes.
* Palette Based - 8 bit color lookup tables are used for RGB values  
* Drawing Mode - when actively editing pixels
* Composite Mode - a mode where layouts and animation involves positioning and reusing cels/visual elements.
* Canvas - a rectangle used to define the bounding rectangle size of your animations. 
* Visual Element - an image that can have more than one instance across the entire showcase.
* Cel - an instance of a visual element used in an animation.
* Index Based Transparent indices (0 for global and right button color for brush operations)
* Transparent - the color is not drawn (not the same as translucency)
* Pixels - are really 8 bit values to lookup RGB values from the contextual palette used.
* Contextual Palette - animations can have multiple palettes each associated with a layer.
* UNDO/REDO - BPainT has a very (primitive) undo system modeled directly on how Deluxe Paint handled it.

#### Startup 

![Startup](https://github.com/lardratboy/BPainT-Preview/raw/master/img/FirstRunScreen.PNG?raw=true)

### (New Animation Showcase dialog)

Every showcase must have at least one Animation.

![NewAnimationShowcaseDialog](https://github.com/lardratboy/BPainT-Preview/raw/master/img/NewAnimationShowcaseDialog.PNG?raw=true)

![AfterNewShowcaseDialogMainView](https://github.com/lardratboy/BPainT-Preview/raw/master/img/AfterNewShowcaseDialogMainView.PNG?raw=true)

BPainT starts out initially without all the toolbars enabled, as the user is expected to have a preferences to how they layout their screens for work. Above you see there are 3 main nested windows - two canvas views and an animation grid. 

![Palette](https://github.com/lardratboy/BPainT-Preview/raw/master/img/PopupPaletteWindow.PNG?raw=true)

Shown above is the Palette tool window. The palette is displayed as a 16x16 grid (index 0 is the upper left and index 255 is the lower right).  The indicators at the bottom of the window show the index bound to left and right mouse button operations.

![Index 0 as Chromakey](https://github.com/lardratboy/BPainT-Preview/raw/master/img/EditColorIndex0ToBeTheEGAMagenta.PNG?raw=true)

Above is an example of setting color index 0 to RGB(170,0,170). To make this edit press (lowercase p) or double-click on the Palette window.

## Two Transparent colors concepts

* Global transparent index is palette index 0 
* The Brush transparent index is defined by right button index.

### Global Transparent (index 0)

Index 0 is special in BPainT it is tightly bound to the concept of a pixel that is never stored, basically emptiness or a fully transparent color.

### Brush transparent index 

The right button drawing index is special for pickup brush operations, it's used as the transparent index for the picked up brush. Usually the right button index is set to index 0 which makes the brush and global transparent and the right operations act like an eraser in the shape of the brush picked up. 

## Next lets toggle on some features

This will help show some core working elements and make things look more like other tools.

![Show Visual Element ID's](https://github.com/lardratboy/BPainT-Preview/raw/master/img/ShowVisualElementIDAnimGridContextMenu.PNG?raw=true)

Right click on an animation grid element to bring up a context menu, the item we are wanting to enable is *Show Visual Element ID's* .  When enabled this will allow you to see which frames are sharing the same visual.

## Turn on some toolbars

In the UI menu, you will find a number of toolbar options, lets turn them on and dock them right under the menu bar.

![Turn on all the things](https://github.com/lardratboy/BPainT-Preview/raw/master/img/AllToolBarsDockedAndCheckerboardBG.PNG?raw=true)

You will also notice I enabled the checker backdrop it helps in understanding how palette index 0 is treated as the global transparent index.

### Automatic window focus

Live brush tracking and automatic focus - when enabled the program will set the focus to the window under the cursor. This  allows the views that have hotkeys that need to be associated with the active view to feel more natural.  This zoom to and scroll to hotkeys are the two that need this most. 

### Two modes only one has an UNDO 

* Composite mode has no undo - this is something I never added
* Active drawing is the only mode that has undo.  And that this undo modeled around how Deluxe Paint handled undo.  Basically UNDO in BPainT is an UNDO/REDO operation. 




** more soon **








