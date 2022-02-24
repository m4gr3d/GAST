package org.godotengine.plugin.gast.activity

import android.os.Bundle
import android.util.Log
import org.godotengine.godot.Godot

class GastGodot : Godot() {

    override fun onCreate(bundle: Bundle?) {
        retainInstance = true
        super.onCreate(bundle)
        Log.d("FHK", "Godot ON CREATE")
    }
}
