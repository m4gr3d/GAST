package org.godotengine.plugin.gast.samples.xrapp.xrmediaviewer

import android.os.Bundle
import android.view.View
import androidx.activity.OnBackPressedCallback
import androidx.activity.viewModels
import androidx.slidingpanelayout.widget.SlidingPaneLayout
import org.godotengine.plugin.gast.samples.xrapp.xrmediaviewer.photos.PhotosViewModel
import org.godotengine.plugin.gast.xrapp.XrActivity

class MainActivity : XrActivity() {

    private val photosModel: PhotosViewModel by viewModels()

    override fun isXrEnabled() = BuildConfig.FLAVOR == "ovr"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val slidingPaneLayout = findViewById<SlidingPaneLayout>(R.id.sliding_pane_layout)
        onBackPressedDispatcher.addCallback(this, SlidingPaneBackPressedCallback(slidingPaneLayout))

        photosModel.getSelectedPhoto().observe(this) {
            slidingPaneLayout.open()
        }
    }

    class SlidingPaneBackPressedCallback(private val slidingPaneLayout: SlidingPaneLayout) :
        OnBackPressedCallback(
            // Set the default 'enabled' state to true only if it is slideable (i.e., the panes
            // are overlapping) and open (i.e., the detail pane is visible).
            slidingPaneLayout.isSlideable && slidingPaneLayout.isOpen),
        SlidingPaneLayout.PanelSlideListener {

        init {
            slidingPaneLayout.addPanelSlideListener(this)
        }

        override fun handleOnBackPressed() {
            // Return to the list pane when the system back button is pressed.
            slidingPaneLayout.closePane()
        }

        override fun onPanelSlide(panel: View, slideOffset: Float) {}

        override fun onPanelOpened(panel: View) {
            // Intercept the system back button when the detail pane becomes visible.
            isEnabled = true
        }

        override fun onPanelClosed(panel: View) {
            // Disable intercepting the system back button when the user returns to the
            // list pane.
            isEnabled = false
        }

    }
}
