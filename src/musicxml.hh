#ifndef MUSICXML_HH_
# define MUSICXML_HH_

# include <string>
# include <vector>
# include "note_detection.hh"

void musicxml(std::string filename,
	      std::vector<Note>& notes);

#endif // !MUSICXML_HH_
