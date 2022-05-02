package org.godotengine.plugin.gast.samples.xrapp.xrmediaviewer.photos

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.activityViewModels
import org.godotengine.plugin.gast.samples.xrapp.xrmediaviewer.R
import org.godotengine.plugin.gast.xrapp.XrFragment

/**
 * Displays the list of photos.
 */
class PhotosListFragment : XrFragment() {

    private val photosModel: PhotosViewModel by activityViewModels()

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        bundle: Bundle?
    ): View? {
        return inflater.inflate(R.layout.photos_list_fragment, container, false)
    }

    override fun onViewCreated(view: View, bundle: Bundle?) {
        super.onViewCreated(view, bundle)
        photosModel.getPhotos().observe(viewLifecycleOwner, { photos ->
            // Update the photos display UI
        })
    }
}
