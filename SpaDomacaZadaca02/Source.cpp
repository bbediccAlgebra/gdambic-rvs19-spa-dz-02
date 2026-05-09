#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <ctime>

using namespace sf;

const int VELICINA_CELIJE = 10;
const int SIRINA = 100;
const int VISINA = 75;
const int PROZOR_W = SIRINA * VELICINA_CELIJE;
const int PROZOR_H = VISINA * VELICINA_CELIJE;
const float VRIJEME_UPDATE = 2.0f;

struct Celija {
	bool ziva;
	bool prijeZiva;
	float anim;
};

std::vector<Celija> mreza(SIRINA * VISINA);
std::vector<Celija> sljedeca(SIRINA * VISINA);

int brojSusjeda(int x, int y) {
	int broj = 0;
	for (int dy = -1; dy <= 1; dy++) {
		for (int dx = -1; dx <= 1; dx++) {
			if (dx == 0 && dy == 0) continue;
			int nx = (x + dx + SIRINA) % SIRINA;
			int ny = (y + dy + VISINA) % VISINA;
			if (mreza[ny * SIRINA + nx].ziva) broj++;
		}
	}
	return broj;
}

void update() {
	for (int y = 0; y < VISINA; y++) {
		for (int x = 0; x < SIRINA; x++) {
			int n = brojSusjeda(x, y);
			Celija c = mreza[y * SIRINA + x];
			Celija nc;
			nc.prijeZiva = c.ziva;
			nc.anim = 0.0f;

			if (c.ziva && (n == 2 || n == 3)) {
				nc.ziva = true;
			}
			else if (!c.ziva && n == 3) {
				nc.ziva = true;
			}
			else {
				nc.ziva = false;
			}

			sljedeca[y * SIRINA + x] = nc;
		}
	}
	mreza = sljedeca;
}

void randomiziraj() {
	std::mt19937 rng((unsigned)time(0));
	std::uniform_int_distribution<int> dist(0, 99);
	for (int i = 0; i < SIRINA * VISINA; i++) {
		mreza[i].ziva = (dist(rng) < 25);
		mreza[i].prijeZiva = false;
		mreza[i].anim = 0.0f;
	}
}

int main() {
	RenderWindow prozor(VideoMode(PROZOR_W, PROZOR_H), "Game of Life");
	prozor.setFramerateLimit(60);

	randomiziraj();

	RectangleShape sjaj(Vector2f((float)(VELICINA_CELIJE + 4), (float)(VELICINA_CELIJE + 4)));
	RectangleShape celija(Vector2f((float)(VELICINA_CELIJE - 2), (float)(VELICINA_CELIJE - 2)));

	VertexArray linije(Lines);
	for (int x = 0; x <= SIRINA; x++) {
		linije.append(Vertex(Vector2f((float)(x * VELICINA_CELIJE), 0.0f), Color(30, 35, 50)));
		linije.append(Vertex(Vector2f((float)(x * VELICINA_CELIJE), (float)PROZOR_H), Color(30, 35, 50)));
	}
	for (int y = 0; y <= VISINA; y++) {
		linije.append(Vertex(Vector2f(0.0f, (float)(y * VELICINA_CELIJE)), Color(30, 35, 50)));
		linije.append(Vertex(Vector2f((float)PROZOR_W, (float)(y * VELICINA_CELIJE)), Color(30, 35, 50)));
	}

	Color zelena(80, 230, 120);
	Color crvena(230, 70, 70);

	Clock sat;
	Clock satFrame;

	while (prozor.isOpen()) {
		Event dogadjaj;
		while (prozor.pollEvent(dogadjaj)) {
			if (dogadjaj.type == Event::Closed)
				prozor.close();
		}

		float dt = satFrame.restart().asSeconds();

		if (sat.getElapsedTime().asSeconds() >= VRIJEME_UPDATE) {
			update();
			sat.restart();
		}

		for (int i = 0; i < SIRINA * VISINA; i++) {
			mreza[i].anim += dt / VRIJEME_UPDATE;
			if (mreza[i].anim > 1.0f) mreza[i].anim = 1.0f;
		}

		prozor.clear(Color(15, 18, 28));
		prozor.draw(linije);

		for (int y = 0; y < VISINA; y++) {
			for (int x = 0; x < SIRINA; x++) {
				Celija c = mreza[y * SIRINA + x];

				Color boja;
				float alpha = 0.0f;

				if (c.ziva && c.prijeZiva) {
					boja = zelena;
					alpha = 1.0f;
				}
				else if (c.ziva && !c.prijeZiva) {
					boja = zelena;
					alpha = c.anim;
				}
				else if (!c.ziva && c.prijeZiva) {
					boja = crvena;
					alpha = 1.0f - c.anim;
				}

				if (alpha > 0.01f) {
					sjaj.setFillColor(Color(boja.r, boja.g, boja.b, (Uint8)(alpha * 60)));
					sjaj.setPosition((float)(x * VELICINA_CELIJE - 2), (float)(y * VELICINA_CELIJE - 2));
					prozor.draw(sjaj);

					celija.setFillColor(Color(boja.r, boja.g, boja.b, (Uint8)(alpha * 255)));
					celija.setPosition((float)(x * VELICINA_CELIJE + 1), (float)(y * VELICINA_CELIJE + 1));
					prozor.draw(celija);
				}
			}
		}

		prozor.display();
	}

	return 0;
}
