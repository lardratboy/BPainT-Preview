# Instructions

todo - video 

## Introduction 

Like many bespoke tools, BPainT has a bit of a learning curve and has some rough edges (and sadly even some bugs). BPainT is no different and was made for artists who had familiarity with similar tools or who were given hands on training in how to use. I will look into making some YouTube videos to serve this purpose. In the mean time hopefully a few screen shots and a brief description of some items will help.

### Some terms

* Showcase - a collection of one or more animations (bpt files are showcase containers).
* Animation - a collection of layer layers with animation frames.
* Annotation (or Text Annotation) - an editable text blob for scripting or notes.
* Palette - an 8 bit CLUT table for RGB values  
* Drawing Mode - when actively editing pixels
* Composite Mode - a mode where layouts and animation involves positioning and reusing cels/visual elements.
* Canvas - a rectangle used to define the bounding rectangle size of your animations. 
* Visual Element - an image that can have more than one instance across the entire showcase.
* Cel - an instance of a visual element used in an animation.

#### Startup 

![Startup](https://github.com/lardratboy/BPainT-Preview/raw/master/img/FirstRunScreen.PNG?raw=true)

#### (New Animation Showcase dialog)

Every showcase must have at least one Animation.

![NewAnimationShowcaseDialog](https://github.com/lardratboy/BPainT-Preview/raw/master/img/NewAnimationShowcaseDialog.PNG?raw=true)

![AfterNewShowcaseDialogMainView](https://github.com/lardratboy/BPainT-Preview/raw/master/img/AfterNewShowcaseDialogMainView.PNG?raw=true)

BPainT starts out initially without all the toolbars enabled, as the user is expected to have a preferences to how they layout their screens for work. Above you see there are 3 main nested windows - two canvas views (the bright red areas, don't worry you have control over these colors) and an animation grid. 

![Palette](https://github.com/lardratboy/BPainT-Preview/raw/master/img/PopupPaletteWindow.PNG?raw=true)

Shown above the palette window is arranged in a 16x16 grid with indicators for the color associated with what color will be applied when the left button is held while drawing and you guessed it the one on the right is for right button drawing.  Lets start by changing the color that will be used for our transparency (think chromakey but an index of 0), we want it to be something we can easily notice and won't be using for drawing. 

![Index 0 as Chromakey](https://github.com/lardratboy/BPainT-Preview/raw/master/img/EditColorIndex0ToBeTheEGAMagenta.PNG?raw=true)

Above is an example of setting color index 0 to RGB(170,0,170).  To make this edit press (lowercase p) or double click on the Palette window.

#### Next lets turn on some features

this will help show some core working elements and make things look more like other tools.

![Show Visual Element ID's](https://github.com/lardratboy/BPainT-Preview/raw/master/img/ShowVisualElementIDAnimGridContextMenu.PNG?raw=true)

This toggle will show the current element id for each cel. An animation is composed of layers and frames animation cel that have Visual elements can be tracked internally 






** more soon **








