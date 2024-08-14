package org.godotengine.plugin.gast.samples.spatialapp

import android.os.Bundle
import org.godotengine.plugin.gast.xrapp.XrActivity

abstract class MainActivity : XrActivity() {
    companion object {
        val TAG = MainActivity::class.java.simpleName
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val testFragment = TestFragment()
        val videoFragment = VideoFragment()
        supportFragmentManager
            .beginTransaction()
            .add(R.id.gast_fragment_holder, testFragment)
            .add(R.id.gast_video_fragment_holder, videoFragment)
            .commitAllowingStateLoss()
    }
}
