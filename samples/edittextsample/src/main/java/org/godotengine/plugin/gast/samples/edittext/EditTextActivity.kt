package org.godotengine.plugin.gast.samples.edittext

import android.os.Bundle
import org.godotengine.plugin.gast.xrapp.XrActivity

class EditTextActivity : XrActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val editTextFragment = EditTextFragment()
        supportFragmentManager
            .beginTransaction()
            .add(R.id.fragment_container, editTextFragment)
            .commitAllowingStateLoss()
    }

    override fun isXrEnabled() = true
}
