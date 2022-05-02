package org.godotengine.plugin.gast.samples.xrapp.xrmediaviewer.photos

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel

class PhotosViewModel: ViewModel() {

    private val photos: MutableLiveData<List<PhotoData>> by lazy {
        MutableLiveData<List<PhotoData>>().also {
            loadOnDevicePhotos()
        }
    }

    private val selectedPhoto = MutableLiveData<PhotoData>()

    fun selectPhoto(photoData: PhotoData) {
        selectedPhoto.value = photoData
    }

    fun getSelectedPhoto(): LiveData<PhotoData> = selectedPhoto

    fun getPhotos(): LiveData<List<PhotoData>> = photos

    private fun loadOnDevicePhotos() {
        TODO("Complete implementation")
    }
}
