#include "musicxml.hh"
#include "tools.hh"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>

bool by_x(Note a,
          Note b)
{
  return a.x < b.x;
}

bool by_y(Piste a,
          Piste b)
{
  return a.y < b.y;
}

static char get_step(Pitch note)
{
  return 'A' + note;
}

void musicxml(std::string filename,
              std::vector<Note>& notes,
              std::vector<Piste>& pistes)
{
  std::ofstream of;

  of.open(filename.c_str());

  if (of.good())
  {
    of << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><!DOCTYPE score-partwise PUBLIC \"-//Recordare//DTD MusicXML 2.0 Partwise//EN\" \"http://www.musicxml.org/dtds/partwise.dtd\"><score-partwise version=\"2.0\">" << std::endl;



    mysort(notes, by_x);
    mysort(pistes, by_y);

    of << "<part-list>" << std::endl
       << "<score-part id=\"P1\"><part-name></part-name></score-part>" << std::endl
       << "<score-part id=\"P2\"><part-name></part-name></score-part>" << std::endl

       << "</part-list>" << std::endl;

    for (int key = 0; key < 2; ++key)
    {
      int measure = 1;
      int t = 0;

      of << "<part id=\"P"<< key + 1 <<"\">" << std::endl;
      of << "<measure number=\"" << measure++ << "\">" << std::endl;
      of << "<attributes>" << std::endl
         << "<divisions>1</divisions>" << std::endl
         << "<key>" << std::endl
	 << "<fifths>0</fifths>" << std::endl
         << "</key>" << std::endl
         << "<time>" << std::endl
         << "<beats>4</beats>" << std::endl
         << "<beat-type>4</beat-type>" << std::endl
         << "</time>" << std::endl
         << "<clef>" << std::endl;
      if (key == 0)
      {
	of << "<sign>G</sign>" << std::endl;
	of << "<line>2</line>" << std::endl;
      }
      else
      {
	of << "<sign>F</sign>" << std::endl;
	of << "<line>4</line>" << std::endl;
      }
        of << "</clef>" << std::endl
         << "</attributes>" << std::endl;

      for (int current_piste = 0;
           current_piste < pistes.size();
           ++ current_piste)
      {
        if (pistes[current_piste].key == key)
        {
          for (size_t k = 0; k < notes.size(); ++k)
          {
            if ((notes[k].y >= pistes[current_piste].y) &&
                (notes[k].y <= pistes[current_piste].y + pistes[current_piste].height))
            {
              t += notes[k].duration;
              of << "<note>" << std::endl
                 << "<pitch>" << std::endl
                 << "<step>" << get_step(notes[k].note) << "</step>" << std::endl
                 << "<octave>"<< 4+notes[k].octave-key << "</octave>" << std::endl
                 << "</pitch>" << std::endl
                 << "<duration>"<< 1 + (int)log2(notes[k].duration) <<"</duration>" << std::endl
                 << "<type>";
              switch (notes[k].duration)
              {
              case DURATION_CROCHE:
                of << "eighth";
                break;
              case DURATION_NOIRE:
                of << "quarter";
                break;
              case DURATION_BLANCHE:
                of << "half";
                break;
              case DURATION_RONDE:
                of << "whole";
                break;
              }
              of << "</type>" << std::endl
                 << "</note>" << std::endl;
              if (t >= 8)
              {
                of << "</measure>" << std::endl;
                of << "<measure number=\"" << measure++ << "\">" << std::endl;
                t = 0;
              }
            }
          }
        }
      }
      of << "</measure>" << std::endl;
      of << "</part>" << std::endl;
    }
    of << "</score-partwise>" << std::endl;
    of.close();
  }
}

