package org.godotengine.plugin.gast.samples.spatialapp

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import org.godotengine.plugin.gast.xrapp.XrFragment

class TestFragment: XrFragment() {
    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        bundle: Bundle?
    ): View? {
        return inflater.inflate(R.layout.test_fragment, container, false)
    }
}
