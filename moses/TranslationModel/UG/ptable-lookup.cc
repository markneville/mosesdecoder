#include "mmsapt.h"
#include "moses/TranslationModel/PhraseDictionaryTreeAdaptor.h"
#include "moses/TranslationTask.h"
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/tokenizer.hpp>
#include <boost/shared_ptr.hpp>
#include <algorithm>
#include <iostream>

using namespace Moses;
using namespace bitext;
using namespace std;
using namespace boost;

vector<FactorType> fo(1,FactorType(0));

class SimplePhrase : public Moses::Phrase
{
  vector<FactorType> const m_fo; // factor order
public:
  SimplePhrase(): m_fo(1,FactorType(0)) {}

  void init(string const& s)
  {
    istringstream buf(s); string w;
    while (buf >> w)
      {
	Word wrd;
	this->AddWord().CreateFromString(Input,m_fo,StringPiece(w),false,false);
      }
  }
};

class TargetPhraseIndexSorter
{
  TargetPhraseCollection const& my_tpc;
  CompareTargetPhrase cmp;
public:
  TargetPhraseIndexSorter(TargetPhraseCollection const& tpc) : my_tpc(tpc) {}
  bool operator()(size_t a, size_t b) const
  {
    return cmp(*my_tpc[a], *my_tpc[b]);
  }
};

int main(int argc, char* argv[])
{
  Parameter params;
  if (!params.LoadParam(argc,argv) || !StaticData::LoadDataStatic(&params, argv[0]))
    exit(1);

  StaticData const& global = StaticData::Instance();
  global.SetVerboseLevel(0);
  vector<FactorType> ifo = global.GetInputFactorOrder();

  PhraseDictionary* PT = PhraseDictionary::GetColl()[0];
  Mmsapt* mmsapt = dynamic_cast<Mmsapt*>(PT);
  PhraseDictionaryTreeAdaptor* pdta = dynamic_cast<PhraseDictionaryTreeAdaptor*>(PT);
  // vector<FeatureFunction*> const& ffs = FeatureFunction::GetFeatureFunctions();

  if (!mmsapt && !pdta)
    {
      cerr << "Phrase table implementation not supported by this utility." << endl;
      exit(1);
    }

  string line;
  while (true)
    {
      boost::shared_ptr<Sentence> phrase(new Sentence);
      if (!phrase->Read(cin,ifo)) break;
      boost::shared_ptr<TranslationTask> ttask;
      ttask = TranslationTask::create(phrase);
      if (pdta)
	{
	  pdta->InitializeForInput(ttask);
	  // do we also need to call CleanupAfterSentenceProcessing at the end?
	}
      Phrase& p = *phrase;

      cout << p << endl;
      TargetPhraseCollection const* trg = PT->GetTargetPhraseCollectionLEGACY(ttask,p);
      if (!trg) continue;
      vector<size_t> order(trg->GetSize());
      for (size_t i = 0; i < order.size(); ++i) order[i] = i;
      sort(order.begin(),order.end(),TargetPhraseIndexSorter(*trg));
      size_t k = 0;
      // size_t precision =
      cout.precision(2);

      vector<string> fname;
      if (mmsapt)
      	{
      	  fname = mmsapt->GetFeatureNames();
      	  cout << "   ";
      	  BOOST_FOREACH(string const& fn, fname)
      	    cout << " " << format("%10.10s") % fn;
      	  cout << endl;
      	}

      BOOST_FOREACH(size_t i, order)
      	{
      	  Phrase const& phr = static_cast<Phrase const&>(*(*trg)[i]);
      	  cout << setw(3) << ++k << " " << phr << endl;
      	  ScoreComponentCollection const& scc = (*trg)[i]->GetScoreBreakdown();
	  size_t start = PT->GetIndex();
	  size_t stop  = start + PT->GetNumScoreComponents();
      	  FVector const& scores = scc.GetScoresVector();
      	  cout << "   ";
      	  for (size_t k = start; k < stop; ++k)
      	    {
	      size_t j = k-start;
	      float f = (mmsapt ? mmsapt->isLogVal(j) ? exp(scores[k]) : scores[k]
			 : scores[k] < 0 ? exp(scores[k]) : scores[k]);
	      string fmt = (mmsapt && mmsapt->isInteger(j)) ? "%10d" : "%10.8f";
	      cout << " " << format(fmt) % f;
      	    }
      	  cout << endl;
      	}
      PT->Release(trg);
    }
  exit(0);
}



