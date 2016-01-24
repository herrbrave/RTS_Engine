#ifndef __UI_H__
#define __UI_H__

#include<algorithm>
#include<memory>
#include<SDL.h>
#include<SDL_ttf.h>
#include<string>

#include"SDL_Helpers.h"

enum Location {
	MIDDLE,
	TOP,
	PAGE_START
};

class ProgressBar {
private:
	Location mLocation;
	int mMaxProgress;
	int mProgress;
public:
	ProgressBar(int maxProgress) : ProgressBar(maxProgress, MIDDLE) {}
	ProgressBar(int maxProgress, Location location) {
		mMaxProgress = mProgress = maxProgress;
		mLocation = location;
	}

	int getProgress() {
		return mProgress;
	}

	int getMaxProgress() {
		return mMaxProgress;
	}

	void setProgress(int progress) {
		mProgress = std::min(std::max(0, progress), mMaxProgress);
	}

	void draw(SDL_Renderer* renderer, SDL_Rect* rect) {
		// padding is two pixels.
		int padding(2);
		int width(rect->w);
		int height(8);
		float percentile((float)mProgress / (float)mMaxProgress);
		int innerwidth((width - 2 * padding) * percentile);
		SDL_Rect outerRect{ 0, 0, 0, 0 };
		SDL_Rect innerRect{ 0, 0, 0, 0 };

		if (mLocation == MIDDLE) {
			outerRect.x = rect->x;
			outerRect.y = rect->y + (rect->h / 2) - (height / 2);
			outerRect.w = width;
			outerRect.h = height;

			innerRect.x = rect->x;
			innerRect.y = rect->y + (rect->h / 2) - (height / 2) + padding;
			innerRect.w = innerwidth;
			innerRect.h = height - 2 * padding;
		}
		else {
			outerRect.x = rect->x - (rect->w / 2);
			outerRect.y = rect->y - (rect->h / 2) - height;
			outerRect.w = width;
			outerRect.h = height;

			innerRect.x = rect->x - (rect->w / 2) + padding;
			innerRect.y = rect->y - (rect->h / 2) - height + padding;
			innerRect.w = innerwidth;
			innerRect.h = height - 2 * padding;
		}

		SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
		SDL_RenderFillRect(renderer, &outerRect);

		float redScalar(((float)mMaxProgress - (float)mProgress) / (float)mMaxProgress);
		float greenScalar((float)mProgress / (float)mMaxProgress);

		SDL_SetRenderDrawColor(renderer, 255 * redScalar, 255 * greenScalar, 0, 255);
		SDL_RenderFillRect(renderer, &innerRect);
	}
};


class Label {
private:
	std::unique_ptr<SDL_Texture, SDL_DELETERS> mTexture;
	std::string mText;
	Location mLocation;

public:
	Label(std::string text, TTF_Font* font, SDL_Renderer* renderer, Location location) {
		mText = text;
		mLocation = location;

		SDL_Color textColor = { 255, 255, 255, 255 };
		SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
		mTexture = std::unique_ptr<SDL_Texture, SDL_DELETERS>(
			SDL_CreateTextureFromSurface(renderer, surface)
			);
		SDL_FreeSurface(surface);
	}

	void draw(SDL_Renderer* renderer, SDL_Rect* location) {
		SDL_Rect textRect{ 0, 0, 0, 0 };
		SDL_QueryTexture(mTexture.get(), nullptr, nullptr, &textRect.w, &textRect.h);

		SDL_Rect drawLocation;
		switch (mLocation) {
		case TOP:
			drawLocation = {
				location->x - textRect.w / 2,
				location->y - textRect.h - 25,
				textRect.w,
				textRect.h
			};
			break;
		case PAGE_START:
			drawLocation = {
				location->x,
				location->y,
				textRect.w,
				textRect.h
			};
			break;
		case MIDDLE:
			drawLocation = {
				location->x + (location->w / 2) - (textRect.w / 2),
				location->y + (location->h / 2) - (textRect.h / 2),
				textRect.w,
				textRect.h
			};
			break;
		default:
			throw std::runtime_error("Invalid location for lable: " + mLocation);
			break;
		}

		

		SDL_RenderCopy(renderer, mTexture.get(), nullptr, &drawLocation);
	}
};

#endif