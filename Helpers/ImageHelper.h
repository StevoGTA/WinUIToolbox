//----------------------------------------------------------------------------------------------------------------------
//	ImageHelper.h			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "FrameworkElementHelper.h"

#include "winrt\Microsoft.UI.Xaml.Controls.h"
#include "winrt\Windows.Storage.Streams.h"

#include <cstdint>
#include <vector>

using Image = winrt::Microsoft::UI::Xaml::Controls::Image;
using IRandomAccessStream = winrt::Windows::Storage::Streams::IRandomAccessStream;

//----------------------------------------------------------------------------------------------------------------------
// MARK: ImageHelper

class ImageHelper : public FrameworkElementHelper<Image, ImageHelper> {
	// ThemedSVG
	public:
		struct ThemedSVG {
			// MARK: Properties
			public:
				winrt::hstring	mLightURI;
				winrt::hstring	mDarkURI;
		};

	// Methods
	public:
						// Lifecycle methods
						ImageHelper(Image image) : FrameworkElementHelper(image) {}

						// Instance methods
		ImageHelper&	setImageData(const IRandomAccessStream& randomAccessStream);
		ImageHelper&	setImageData(const std::vector<uint8_t>& imageData);
		ImageHelper&	setImageData(const uint8_t* imageDataPtr, size_t byteCount);

		ImageHelper&	set(const ThemedSVG& themedSVG);

		Image			getImage() const
							{ return getFrameworkElement(); }
};
