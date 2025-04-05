//----------------------------------------------------------------------------------------------------------------------
//	ImageHelper.h			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "FrameworkElementHelper.h"

#include "winrt\Microsoft.UI.Dispatching.h"
#include "winrt\Microsoft.UI.Xaml.Media.Imaging.h"
#include "winrt\Windows.Foundation.h"
#include "winrt\Windows.Storage.Streams.h"
#include "winrt\Windows.System.Threading.h"

using BitmapImage = winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage;
using DataWriter = winrt::Windows::Storage::Streams::DataWriter;
using IAsyncAction = winrt::Windows::Foundation::IAsyncAction;
using IRandomAccessStream = winrt::Windows::Storage::Streams::IRandomAccessStream;
using Image = winrt::Microsoft::UI::Xaml::Controls::Image;
using InMemoryRandomAccessStream = winrt::Windows::Storage::Streams::InMemoryRandomAccessStream;
using ThreadPool = winrt::Windows::System::Threading::ThreadPool;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ImageHelper

class ImageHelper : public FrameworkElementHelper<Image, ImageHelper> {
	// Methods
	public:
						// Lifecycle methods
						ImageHelper(Image image) : FrameworkElementHelper(image) {}
 
						// Instance methods
		ImageHelper&	setImageData(const IRandomAccessStream& randomAccessStream)
							{
								// Setup Image source
								BitmapImage bitmapImage;
								bitmapImage.SetSource(randomAccessStream);

								// Set image
								getImage().Source(bitmapImage);

								return *this;
							}
		ImageHelper&	setImageData(const std::vector<uint8_t>& imageData)
							{
								// Setup
								Image	image = getImage();

								// Run in background
								ThreadPool::RunAsync([imageData, image](const IAsyncAction& asyncAction){
									// Setup stream
									InMemoryRandomAccessStream memoryStream;

									// Write image data to stream
									DataWriter dataWriter(memoryStream.GetOutputStreamAt(0));
									dataWriter.WriteBytes(imageData);
									dataWriter.StoreAsync();
									dataWriter.FlushAsync();
									dataWriter.DetachStream();

									// Switch to UI
									image.DispatcherQueue().TryEnqueue([memoryStream, image](){
										// Set image
										ImageHelper(image).setImageData(memoryStream);
									});
								});

								return *this;
							}
		ImageHelper&	setImageData(const uint8_t* imageDataPtr, size_t byteCount)
							{ return setImageData(std::vector<uint8_t>(imageDataPtr, imageDataPtr + byteCount)); }

		Image			getImage() const
							{ return getFrameworkElement(); }
};
