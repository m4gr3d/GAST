# Godot XRApp Framework Roadmap

This is a tentative roadmap for the Godot XRApp framework.

## Phase 1 - UI Interface Customization

### Version 0.3.0
- Complete `GastFrameLayout` functionality
    - `FrameLayout`-derived implementation that unlocks the ability to provide z-depth positioning
      and animation to UI elements in the layout

### Version 0.4.0
- Complete `GastFragment` implementation
    - Unlocks the use of [responsive layout](https://developer.android.com/guide/topics/large-screens/migrate-to-responsive-layouts) when in VR

### Version 0.5.0
- Complete `GastSurfaceView` implementation
    - E.g: Enables video playback support

### Version 0.6.0
- Complete `GastImageView` implementation
    - `ImageView`-derived implementation with the ability to specify and load a gltf 3d-model
      when in VR.
        - Fallback to the 2D image source when no 3d-model is specified or when in pancake mode.

## Phase 2 - Leveraging VR Extensions
- Enhanced Passthrough support
    - Implement `GastPassthroughView` - an Android view through which bounds passthrough can be
      rendered
- [Spatial Anchors](https://developer.oculus.com/experimental/spatial-anchors-overview/) support

## Phase 3 - Environments Customization
- Enable custom environments loading
    - Provide a template developers can build from so as to have full control of the environment
      while preserving base functionality expected by the framework
