#ifndef __SERIALIZER_H__
#define	__SERIALIZER_H__

#include<rapidjson\prettywriter.h>
#include<rapidjson\document.h>

class Serializer {
public:
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer{ stringBuffer };

	Serializer() {
	}
};

class Serializable {
public:
	virtual void serialize(Serializer& serializer) const = 0;
};

#endif // !__SERIALIZER_H__
