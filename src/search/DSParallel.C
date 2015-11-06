#include<stdlib.h>
#include<math.h>
#include<DSParallel.h>


DSParallel::DSParallel() {
  totalparams = 1;
  stepSizes = (int *) malloc(sizeof(int) * (totalparams));
  minStepSize = MIN_STEP_SIZE;
  stepSizeReduceFactor = REDUCE_FACTOR;
  inc = true;
  state = NOTSTARTED;
  region = 0;

  for (int i = 0; i < totalparams; i++) {
    struct optInfo opt;
    opt.loop = 0;
    opt.optCode = 0;
    opt.bottom = 0;
    opt.top = 0;
    opts.push_back(opt);
    stepSizes[i] = 1;
  }
}

DSParallel::DSParallel(int _region,
		       int _totalparams, 
		       int *_loopnums,
		       int *_optCode,
		       int *_bottom,
		       int *_top) : totalparams(_totalparams),
				    minPoint(_totalparams),
				    curPoint(_totalparams), 
				    basePoint(_totalparams),
				    explPoint(_totalparams) {

  region = _region;
  stepSizes = (int *) malloc(sizeof(int) * (totalparams));
  minStepSize = MIN_STEP_SIZE;
  stepSizeReduceFactor = REDUCE_FACTOR;
  inc = true;
  state = NOTSTARTED;
  

  for (int i = 0; i < totalparams; i++) {
    struct optInfo opt;
    opt.loop = _loopnums[i];
    opt.optCode = _optCode[i];
    opt.bottom = _bottom[i];
    opt.top = _top[i];
    opts.push_back(opt);
    stepSizes[i] = (int) ceil(((float) 
			       (_top[i] - _bottom[i]) * STEP_SIZE_FRACTION)/100);
  }
}

DSParallel::~DSParallel() {
}

float DSParallel::GetBestValue() {
  if (state == NOTSTARTED || state == INIT)
    return curVal;
  if (minVal < curVal)
    return (minVal < baseVal ? minVal : baseVal);
  else 
    return (curVal < baseVal ? curVal : baseVal);
}

Point DSParallel::GetBestPoint() {
  if (state == NOTSTARTED || state == INIT)
    return curPoint;
  if (minVal < curVal)
    return (minVal < baseVal ? minPoint : basePoint);
  else 
    return (curVal < baseVal ? curPoint : basePoint);
}

bool DSParallel::inRange(Point p) {
  for (int i = 0; i < totalparams; i++) {
    if (p[i] < GetBottom(i) || p[i] > GetTop(i)) 
      return false;
  }
  return true;
}

void DSParallel::MakeExploreMove(int paramnum, bool inc) {
  curPoint = minPoint;
  if (inc) {
    if (explPoint[paramnum] + stepSizes[paramnum] <= GetTop(paramnum)) {
      curPoint.SetVal(paramnum, explPoint[paramnum] + stepSizes[paramnum]);
    }
    else {
      curPoint.SetVal(paramnum, explPoint[paramnum]);
    }
  }
  else {
    if (explPoint[paramnum] - stepSizes[paramnum] >= GetBottom(paramnum)) {
      curPoint.SetVal(paramnum, explPoint[paramnum] - stepSizes[paramnum]);
    }
    else {
      curPoint.SetVal(paramnum, explPoint[paramnum]);
    }
  }
}

#if 0
void DSParallel::MakeExploreMove(int paramnum, bool inc) {
  if (inc) {
    explPoint.Inc(paramnum, stepSize);
    if (!inRange(explPoint)) {
      explPoint.Dec(paramnum, stepSize);
      success = false;
    }
    else 
      success = true;
  }    
  else { 
    if (success) 
      explPoint.Dec(paramnum, 2 * stepSize);
    else
      explPoint.Dec(paramnum, stepSize);
    if (!inRange(explPoint)) {
      explPoint.Inc(paramnum, stepSize);
      success = false;
    }
    else 
      success = true;
  }
  curPoint = explPoint;
}
#endif

void DSParallel::MakePatternMove() {
  for(int i = 0; i < totalparams; i++) {
    int newVal = 2 * minPoint[i] - basePoint[i];
    if (newVal < GetBottom(i))
      newVal = GetBottom(i);
    else if (newVal > GetTop(i)) 
      newVal = GetTop(i);
    explPoint.SetVal(i, newVal);
  }
  basePoint = minPoint;
  minPoint = explPoint;
  curPoint = explPoint;
  baseVal = minVal;
}

#if 0
void DSParallel::MakePatternMove() {
  for (int i = 0; i < totalparams; i++) {
    int newVal = 2 * explPoint.GetVal(i) - basePoint.GetVal(i);
    if (newVal < GetBottom(i))
      newVal = GetBottom(i);
    else if (newVal > GetTop(i)) 
      newVal = GetTop(i);
    explPoint.SetVal(i, newVal);
    basePoint.SetVal(i, curPoint.GetVal(i));
  }
  curPoint = explPoint;
  baseVal = minVal;
}
#endif

void DSParallel::PickInitialPoints() {
  for (int i = 0; i < totalparams; i++) {
    int val = (GetTop(i) - GetBottom(i))/2;
    curPoint.SetVal(i, val); 
  }
}


Point DSParallel::Normalize(Point& p) {
  for (int i = 0; i < totalparams; i++) {
    p.SetVal(i, p[i] - GetBottom(i));
  }
  return p;
}

bool DSParallel::GenParams() {

  switch (state) {
  case NOTSTARTED:
    PickInitialPoints();
    state = INIT;
    return false;
  case INIT:
    baseVal = minVal = explVal = curVal;
    basePoint = curPoint;
    explPoint = curPoint;
    minPoint = curPoint;
    paramnum = 0;
    inc = true;
    MakeExploreMove(paramnum, inc);
    state = EXPLORE;
    return false;
  case EXPLORE:
    if (curVal < minVal) {
      minVal = curVal;
      minPoint = curPoint;
      paramnum++;
      if (paramnum == totalparams) {
	if (minVal < baseVal) {
	  MakePatternMove();
	  state = PATTERN;
	}
	else {
	  stepSizes[paramnum - 1] -= stepSizeReduceFactor;
	  if (stepSizes[paramnum - 1] < minStepSize) {
	    state = DONE;
	    return true;
	  }
	  else {
	    paramnum = 0;
	    inc = true;
	    minPoint = explPoint;
	    minVal = explVal;
	    MakeExploreMove(paramnum, inc);
	    state = EXPLORE;
	  }
	}
      }
      else {
	inc = true;
	MakeExploreMove(paramnum, inc);
      }
    }
    else {
      if (inc) {
	inc = false;
	MakeExploreMove(paramnum, inc);
      }
      else {
	paramnum++;
	if (paramnum == totalparams) {
	  if (minVal < baseVal) {
	    MakePatternMove();
	    state = PATTERN;
	  }
	  else {
 	    stepSizes[paramnum - 1] -= stepSizeReduceFactor;
	    if (stepSizes[paramnum - 1] < minStepSize) {
	      state = DONE;
	      return true;
	    }
	    else {
	      paramnum = 0;
	      inc = true;
	      minPoint = explPoint;
	      minVal = explVal;
	      MakeExploreMove(paramnum, inc);
	      state = EXPLORE;
	    }
	  }
	}
	else {
	  inc = true;
	  MakeExploreMove(paramnum, inc);
	}
      }
    }
    return false;
  case PATTERN:
    minVal = explVal = curVal;
    paramnum = 0;
    inc = true;
    MakeExploreMove(paramnum, inc);
    state = EXPLORE;
    return false;;
  case DONE:
    return true;
  default:
    return false;
  }
}







