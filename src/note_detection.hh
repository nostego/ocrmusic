#ifndef NOTE_DETECTION_HH_
# define NOTE_DETECTION_HH_

# include "symbol_detection.hh"

enum Pitch
{
  PITCH_A,
  PITCH_B,
  PITCH_C,
  PITCH_D,
  PITCH_E,
  PITCH_F,
  PITCH_G,
  SILENCE
};

enum Alter
{
  NONE,
  ALTER_BECAR,
  ALTER_BEMOL,
  ALTER_DIESE
};

enum Duration
{
  DURATION_CROCHE = 1,
  DURATION_NOIRE = 2,
  DURATION_BLANCHE = 4,
  DURATION_RONDE = 8
};

struct Note
{
  Duration duration; // croche, noire, ...
  int octave; // from 0 (grave) to X (aigue)
  Pitch note; // A,B,C,D,E,F,G
  Alter alter; //bemol, becar, diese

  // FIXME: remove it after. Use only x and y and handle pos in xml code.
  int position; // id de la note, plusieurs notes avec le same id

  //=> accord
  int x;
  int y;
};

void detect_notes(cv::Mat& img,
                  std::vector<Line>& lines,
                  std::vector<Piste>& pistes,
                  std::vector<Symbol>& symbols,
                  std::vector<Note>& notes);
#endif // !NOTE_DETECTION_HH_
