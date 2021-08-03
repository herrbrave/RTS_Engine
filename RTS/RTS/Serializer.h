#ifndef __SERIALIZER_H__
#define	__SERIALIZER_H__

#include<fstream>
#include<iostream>
#include<rapidjson\prettywriter.h>
#include<rapidjson\document.h>

class Serializer {
public:
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer{ stringBuffer };

	Serializer() {
	}

	void writeTo(const string& path) {
		std::fstream out;
		out.open(path, std::ios::out | std::ios::trunc);
		out << stringBuffer.GetString() << std::endl;
		out.close();
	}
};

class Serializable {
public:
	virtual void serialize(Serializer& serializer) const = 0;
};

#endif // !__SERIALIZER_H__
