/*
 * ConvertAmount.h
 *
 *  Created on: Mar 16, 2009
 *      Author: tpielech
 */

#ifndef CONVERTAMOUNT_H_
#define CONVERTAMOUNT_H_

#include <QString>
#include "Settings.h"

class ConvertAmount {

public:

	ConvertAmount();
	~ConvertAmount();

	void convertTest();
    QString convertPL(QString input, QString currency);
    QString& descAmount(QString&,QString,QString = QString(),QString = QString(),QString = QString());
    const QStringList hundsList();
    const QStringList endsHundList();
    const QStringList endsTensList();
    const QStringList simpleNumbList();

};

#endif /* CONVERTAMOUNT_H_ */
