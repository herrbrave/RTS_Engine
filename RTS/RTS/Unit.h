#ifndef __UNIT_H__
#define __UNIT_H__

#include"Tile.h"
#include"Ui.h"

enum UnitType {
	WARRIOR,
	WORKER,
	ARCHER
};

class Unit : public Entity {
public:
	Unit(int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, UnitType unitType, std::shared_ptr<TTF_Font> font, std::shared_ptr<SDL_Renderer> renderer, std::shared_ptr<Camera> camera) : Entity(x, y, width, height, r, g, b, camera){
		mUnitType = unitType;
		switch (unitType) {
		case WARRIOR:
			mName.reset(new Label("Warrior", font.get(), renderer.get(), TOP));
			mHealthBar.reset(new ProgressBar(WARRIOR_MAX_HEALTH, TOP));
			break;
		case WORKER:
			mName.reset(new Label("Worker", font.get(), renderer.get(), TOP));
			mHealthBar.reset(new ProgressBar(WORKER_MAX_HEALTH, TOP));
			break;
		case ARCHER:
			mName.reset(new Label("Archer", font.get(), renderer.get(), TOP));
			mHealthBar.reset(new ProgressBar(ARCHER_MAX_HEALTH, TOP));
			break;
		default:
			throw std::runtime_error("Invalid UnitType: " + unitType);
			break;
		}
	}

	void setCurrentTile(std::shared_ptr<Tile> currentTile);
	std::shared_ptr<Tile> getCurrentTile();

	void damage(int damage);
	bool isDead(); 
	
	/* Parent methods */
	virtual void update();
	virtual void draw(std::shared_ptr<SDL_Renderer> renderer);

private:
	UnitType mUnitType;
	std::unique_ptr<Label> mName;
	std::unique_ptr<ProgressBar> mHealthBar;
	std::shared_ptr<Tile> mCurrentTile;
	p_vector2f mVelocity;
};

#endif // !__UNIT_H__
