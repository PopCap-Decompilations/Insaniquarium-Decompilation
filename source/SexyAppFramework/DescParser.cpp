#include "DescParser.h"
#include "..\PakLib\PakInterface.h"

using namespace Sexy;

DescParser::DescParser()
{
	mCmdSep = CMDSEP_SEMICOLON;
}

DescParser::~DescParser()
{
}

bool DescParser::Error(const std::string& theError)
{
	// Original (0x41AA20): only the first error is kept
	if (mError.length() == 0)
		mError = theError;

	return false;
}

DataElement* DescParser::Dereference(const std::string& theString)
{
	std::string aDefineName = StringToUpper(theString);

	DataElementMap::iterator anItr = mDefineMap.find(aDefineName);
	if (anItr != mDefineMap.end())
		return anItr->second;
	else
		return NULL;
}

bool DescParser::IsImmediate(const std::string& theString)
{
	return (((theString[0] >= '0') && (theString[0] <= '9')) || (theString[0] == '-') || 
		(theString[0] == '+') || (theString[0] == '\'') || (theString[0] == '"'));
}

std::string DescParser::Unquote(const std::string& theQuotedString)
{
	if ((theQuotedString[0] == '\'') || (theQuotedString[0] == '"'))
	{
		char aQuoteChar = theQuotedString[0];
		std::string aLiteralString;
		bool lastWasQuote = true;
		bool escaped = false;

		// Original (0x41AC38): the enclosing quotes are skipped (unsigned bounds, so a lone quote gives ""),
		// and backslash escapes left in place by ParseToList are resolved here
		for (ulong i = 1; i < theQuotedString.length() - 1; i++)
		{
			if (escaped)
			{
				aLiteralString += theQuotedString[i];
				escaped = false;
			}
			else if (theQuotedString[i] == aQuoteChar)
			{
				if (lastWasQuote)
					aLiteralString += aQuoteChar;

				lastWasQuote = true;
			}
			else
			{
				if (theQuotedString[i] == '\\')
					escaped = true;
				else
					aLiteralString += theQuotedString[i];

				lastWasQuote = false;
			}
		}

		return aLiteralString;
	}
	else
		return theQuotedString;
}

bool DescParser::GetValues(ListDataElement* theSource, ListDataElement* theValues)
{
	theValues->mElementVector.clear();
	
	for (ulong aSourceNum = 0; aSourceNum < theSource->mElementVector.size(); aSourceNum++)
	{
		if (theSource->mElementVector[aSourceNum]->mIsList)
		{
			ListDataElement* aChildList = new ListDataElement();
			theValues->mElementVector.push_back(aChildList);

			if (!GetValues((ListDataElement*) theSource->mElementVector[aSourceNum], aChildList))
				return false;
		}
		else
		{
			std::string aString = ((SingleDataElement*) theSource->mElementVector[aSourceNum])->mString;

			if (aString.length() > 0)
			{				
				if ((aString[0] == '\'') || (aString[0] == '"'))
				{
					SingleDataElement* aChildData = new SingleDataElement(Unquote(aString));
					theValues->mElementVector.push_back(aChildData);					
				}
				else if (IsImmediate(aString))
				{
					theValues->mElementVector.push_back(new SingleDataElement(aString));
				}
				else
				{
					std::string aDefineName = StringToUpper(aString);

					DataElementMap::iterator anItr = mDefineMap.find(aDefineName);

					if (anItr == mDefineMap.end())
					{
						Error("Unable to Dereference \"" + aString + "\"");
						return false;
					}
					
					theValues->mElementVector.push_back(anItr->second->Duplicate());
				}
			}

			
		}
	}

	return true;
}

std::string DescParser::DataElementToString(DataElement* theDataElement)
{
	if (theDataElement->mIsList)
	{
		ListDataElement* aListDataElement = (ListDataElement*) theDataElement;

		std::string aString = "(";

		for (ulong i = 0; i < aListDataElement->mElementVector.size(); i++)
		{
			if (i != 0)
				aString += ", ";

			aString += DataElementToString(aListDataElement->mElementVector[i]);
		}

		aString += ")";

		return aString;
	}
	else
	{
		SingleDataElement* aSingleDataElement = (SingleDataElement*) theDataElement;

		if (aSingleDataElement->mValue != NULL)
			return aSingleDataElement->mString + "=" + DataElementToString(aSingleDataElement->mValue);
		else
			return aSingleDataElement->mString;
	}
}

bool DescParser::DataToString(DataElement* theSource, std::string* theString)
{
	*theString = "";

	if ((theSource->mIsList) || (((SingleDataElement*) theSource)->mValue != NULL))
		return false;

	std::string aDefName = ((SingleDataElement*) theSource)->mString;

	DataElement* aDataElement = Dereference(aDefName);

	if (aDataElement != NULL)
	{
		if (aDataElement->mIsList)
			return false;

		*theString = Unquote(((SingleDataElement*) aDataElement)->mString);
	}
	else
		*theString = Unquote(aDefName);				

	return true;
}

bool DescParser::DataToInt(DataElement* theSource, int* theInt)
{
	*theInt = 0;

	std::string aTempString;
	if (!DataToString(theSource, &aTempString))
		return false;
	
	if (!StringToInt(aTempString, theInt))
		return false;

	return true;
}

bool DescParser::DataToStringVector(DataElement* theSource, StringVector* theStringVector)
{
	theStringVector->clear();

	ListDataElement aStaticValues;
	ListDataElement* aValues;

	if (theSource->mIsList)
	{
		if (!GetValues((ListDataElement*) theSource, &aStaticValues))
			return false;

		aValues = &aStaticValues;
	}
	else
	{
		std::string aDefName = ((SingleDataElement*) theSource)->mString;

		DataElement* aDataElement = Dereference(aDefName);
		
		if (aDataElement == NULL)
		{
			Error("Unable to Dereference \"" + aDefName + "\"");
			return false;
		}

		if (!aDataElement->mIsList)
			return false;

		aValues = (ListDataElement*) aDataElement;
	}	

	for (ulong i = 0; i < aValues->mElementVector.size(); i++)
	{
		if (aValues->mElementVector[i]->mIsList)
		{
			theStringVector->clear();
			return false;
		}		

		SingleDataElement* aSingleDataElement = (SingleDataElement*) aValues->mElementVector[i];

		theStringVector->push_back(aSingleDataElement->mString);
	}

	return true;
}

bool DescParser::DataToList(DataElement* theSource, ListDataElement* theValues)
{
	if (theSource->mIsList)
	{
		return GetValues((ListDataElement*) theSource, theValues);		
	}

	DataElement* aDataElement = Dereference(((SingleDataElement*) theSource)->mString);
		
	if ((aDataElement == NULL) || (!aDataElement->mIsList))
		return false;

	ListDataElement* aListElement = (ListDataElement*) aDataElement;

	*theValues = *aListElement;

	return true;
}

bool DescParser::DataToIntVector(DataElement* theSource, IntVector* theIntVector)
{
	theIntVector->clear();
	
	StringVector aStringVector;
	if (!DataToStringVector(theSource, &aStringVector))
		return false;	

	for (ulong i = 0; i < aStringVector.size(); i++)
	{		
		int aIntVal;
		if (!StringToInt(aStringVector[i], &aIntVal))
			return false;

		theIntVector->push_back(aIntVal);
	}

	return true;
}

bool DescParser::DataToDoubleVector(DataElement* theSource, DoubleVector* theDoubleVector)
{
	theDoubleVector->clear();
	
	StringVector aStringVector;
	if (!DataToStringVector(theSource, &aStringVector))
		return false;	

	for (ulong i = 0; i < aStringVector.size(); i++)
	{		
		double aDoubleVal;
		if (!StringToDouble(aStringVector[i], &aDoubleVal))
			return false;

		theDoubleVector->push_back(aDoubleVal);
	}

	return true;
}

bool DescParser::ParseToList(const std::string& theString, ListDataElement* theList, bool expectListEnd, int* theStringPos)
{
	// Original (0x41BB70): the newer parser. "key=value" puts the next element or list into the key's
	// mValue; separators only end the current element once it has text (deferred until the next char);
	// escapes are kept as "\\"+char so that Unquote can resolve them.
	bool inSingleQuotes = false;
	bool inDoubleQuotes = false;
	bool escaped = false;
	bool wantTerminateSingleDataElement = false;

	SingleDataElement* aKeySingleDataElement = NULL;
	SingleDataElement* aCurSingleDataElement = NULL;

	int aStringPos = 0;

	if (theStringPos == NULL)
		theStringPos = &aStringPos;

	while (*theStringPos < (int) theString.length())
	{
		bool addSingleChar = false;
		char aChar = theString[(*theStringPos)++];

		bool isSeperator = (aChar == ' ') || (aChar == '\t') || (aChar == '\n') || (aChar == ',');

		if (escaped)
		{
			// the escaped char is added as-is; it does not honour wantTerminateSingleDataElement
			addSingleChar = true;
		}
		else
		{
			if ((aChar == '\'') && (!inDoubleQuotes))
				inSingleQuotes = !inSingleQuotes;
			else if ((aChar == '"') && (!inSingleQuotes))
				inDoubleQuotes = !inDoubleQuotes;

			if (aChar == '\\')
			{
				escaped = true;
			}
			else if ((!inSingleQuotes) && (!inDoubleQuotes))
			{
				if (aChar == ')')
				{
					if (expectListEnd)
						return true;
					else
					{
						Error("Unexpected List End");
						return false;
					}
				}
				else if (aChar == '(')
				{
					if (wantTerminateSingleDataElement)
					{
						aCurSingleDataElement = NULL;
						wantTerminateSingleDataElement = false;
					}

					if (aCurSingleDataElement != NULL)
					{
						Error("Unexpected List Start");
						return false;
					}
					else
					{
						ListDataElement* aChildList = new ListDataElement();

						if (!ParseToList(theString, aChildList, true, theStringPos))
							return false;

						if (aKeySingleDataElement != NULL)
						{
							aKeySingleDataElement->mValue = aChildList;
							aKeySingleDataElement = NULL;
						}
						else
							theList->mElementVector.push_back(aChildList);
					}
				}
				else if (aChar == '=')
				{
					// The current element (may be NULL) becomes the key; the next element or list is its value
					aKeySingleDataElement = aCurSingleDataElement;
					wantTerminateSingleDataElement = true;
				}
				else if (isSeperator)
				{
					if ((aCurSingleDataElement != NULL) && (aCurSingleDataElement->mString.length() > 0))
						wantTerminateSingleDataElement = true;
				}
				else
				{
					if (wantTerminateSingleDataElement)
					{
						aCurSingleDataElement = NULL;
						wantTerminateSingleDataElement = false;
					}

					addSingleChar = true;
				}
			}
			else
			{
				if (wantTerminateSingleDataElement)
				{
					aCurSingleDataElement = NULL;
					wantTerminateSingleDataElement = false;
				}

				addSingleChar = true;
			}
		}

		if (addSingleChar)
		{
			if (aCurSingleDataElement == NULL)
			{
				aCurSingleDataElement = new SingleDataElement();

				if (aKeySingleDataElement != NULL)
				{
					aKeySingleDataElement->mValue = aCurSingleDataElement;
					aKeySingleDataElement = NULL;
				}
				else
					theList->mElementVector.push_back(aCurSingleDataElement);
			}

			if (escaped)
			{
				// keep the escape for Unquote
				aCurSingleDataElement->mString += "\\";
				escaped = false;
			}

			aCurSingleDataElement->mString += aChar;
		}
	}

	if (inSingleQuotes)
	{
		Error("Unterminated Single Quotes");
		return false;
	}

	if (inDoubleQuotes)
	{
		Error("Unterminated Double Quotes");
		return false;
	}

	if (expectListEnd)
	{
		Error("Unterminated List");
		return false;
	}

	return true;
}

bool DescParser::ParseDescriptorLine(const std::string& theDescriptorLine)
{
	ListDataElement aParams;
	if (!ParseToList(theDescriptorLine, &aParams, false, NULL))
		return false;
	
	if (aParams.mElementVector.size() > 0)
	{
		if (aParams.mElementVector[0]->mIsList)
		{
			Error("Missing Command");
			return false;
		}

		if (!HandleCommand(aParams))
			return false;
	}

	return true;
}

bool DescParser::LoadDescriptor(const std::string& theFileName)
{
	mCurrentLineNum = 0;
	int aLineCount = 0;
	bool hasErrors = false;

	//Apparently VC6 doesn't have a clear() function for basic_strings
	//mError.clear();
	mError.erase();
	mError.erase(mError.begin());	// the original also does this (0x41C074/0x41C088); a no-op on the now empty string
	mCurrentLine = "";

	PFILE *aStream = p_fopen(theFileName.c_str(),"r");
	if (aStream==NULL)
		return Error("Unable to open file: " + theFileName);	// original 0x41C0B9: reported through the (virtual) Error

	char aBuffChar = 0;

	while (!p_feof(aStream))
	{		
		int aChar;
						
		bool skipLine = false;
		bool atLineStart = true;
		bool inSingleQuotes = false;
		bool inDoubleQuotes = false;
		bool escaped = false; 
		bool isIndented = false;

		for (;;)
		{
			if (aBuffChar != 0)
			{
				aChar = aBuffChar;
				aBuffChar = 0;
			}
			else
			{
				aChar = p_fgetc(aStream);
				if (aChar==EOF)
					break;
			}
			
			if (aChar != '\r')
			{
				if (aChar == '\n')
					aLineCount++;

				if (((aChar == ' ') || (aChar == '\t')) && (atLineStart))
					isIndented = true;

				if ((!atLineStart) || ((aChar != ' ') && (aChar != '\t') && (aChar != '\n')))
				{
					if (atLineStart)
					{
						if ((mCmdSep & CMDSEP_NO_INDENT) && (!isIndented) && (mCurrentLine.size() > 0))
						{
							// Start a new non-indented line
							aBuffChar = aChar;
							break;
						}

						if (aChar == '#')
							skipLine = true;

						atLineStart = false;
					}					

					if (aChar == '\n')		
					{
						isIndented = false;
						atLineStart = true;				
					}

					if ((aChar == '\n') && (skipLine))
					{
						skipLine = false;						
					}
					else if (!skipLine)
					{
						if (aChar == '\\' && (inSingleQuotes || inDoubleQuotes) && !escaped)
							escaped = true;
						else
						{
							if ((aChar == '\'') && (!inDoubleQuotes) && (!escaped))
								inSingleQuotes = !inSingleQuotes;

							if ((aChar == '"') && (!inSingleQuotes) && (!escaped))
								inDoubleQuotes = !inDoubleQuotes;
							
							if ((aChar == ';') && (mCmdSep & CMDSEP_SEMICOLON) && (!inSingleQuotes) && (!inDoubleQuotes))
								break;
							
							if(escaped) // stay escaped for when this is actually parsed
							{
								mCurrentLine += '\\';
								escaped = false;
							}

							if (mCurrentLine.size() == 0)
								mCurrentLineNum = aLineCount + 1;

							mCurrentLine += aChar;
						}
					}
				}
			}
		}

		if (mCurrentLine.length() > 0)
		{
			if (!ParseDescriptorLine(mCurrentLine))
			{
				hasErrors = true;
				break;
			}

			//Apparently VC6 doesn't have a clear() function for basic_strings
			//mCurrentLine.clear();
			mCurrentLine.erase();
		}
	}

	//Apparently VC6 doesn't have a clear() function for basic_strings
	//mCurrentLine.clear();
	mCurrentLine.erase();
	mCurrentLineNum = 0;

	p_fclose(aStream);
	return !hasErrors;
}
