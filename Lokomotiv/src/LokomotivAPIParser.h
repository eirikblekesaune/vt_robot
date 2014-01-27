#ifndef LOKOMOTIV_PARSER_H
#define LOKOMOTIV_PARSER_H

class LokomotivParser{
public:
	LokomotivParser(Lokomotiv *lok) :_lok(lok){};
	void ParseByte(int byte) {};
private:
	Lokomotiv *_lok;
};
#endif

