package org.godotengine.plugin.gast.samples.xrapp.xrmediaviewer

import android.os.Bundle
import android.view.WindowManager
import org.godotengine.plugin.gast.samples.xrapp.xrmediaviewer.videos.VideoControlsFragment
import org.godotengine.plugin.gast.samples.xrapp.xrmediaviewer.videos.VideoPlayerFragment
import org.godotengine.plugin.gast.xrapp.XrActivity

abstract class MainActivity : XrActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val videoPlayerFragment = VideoPlayerFragment()
        val videoControlsFragment = VideoControlsFragment()
        supportFragmentManager
            .beginTransaction()
            .add(R.id.video_player_holder, videoPlayerFragment)
            .add(R.id.video_controls_holder, videoControlsFragment)
            .commitAllowingStateLoss()

        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }
}
