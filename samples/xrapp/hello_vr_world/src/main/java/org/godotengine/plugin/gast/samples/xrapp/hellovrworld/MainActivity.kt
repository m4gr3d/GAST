package org.godotengine.plugin.gast.samples.xrapp.hellovrworld

import android.os.Bundle
import android.view.View
import org.godotengine.plugin.gast.xrapp.GastActivity

class MainActivity : GastActivity() {

    var passthroughEnabled = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        findViewById<View>(R.id.toggle_passthrough).setOnClickListener {
            if (passthroughEnabled) {
                stopPassthrough()
            } else {
                startPassthrough()
            }

            passthroughEnabled = !passthroughEnabled
        }
    }

    override fun isXREnabled() = true
}
