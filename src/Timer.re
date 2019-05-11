type state = {
  seconds: int,
  isTicking: bool,
};

type action =
  | Start
  | Stop
  | Reset
  | Tick;

module Button = {
  [@react.component]
  let make = (~label, ~onClick) =>
    <button className={j|timer__button timer__$label|j} onClick>
      {label |> ReasonReact.string}
    </button>;
};

let updateTitle: string => unit = [%bs.raw
  {|
  function updateTitle(remaining) {
    document.title = "⏰ " + remaining + " ⏰";
  }|}
];

let padNumber = numString =>
  if (numString |> int_of_string < 10) {
    "0" ++ numString;
  } else {
    numString;
  };

let formatTime = seconds => {
  let mins = seconds / 60;
  let minsString = mins |> string_of_int |> padNumber;
  let seconds = seconds mod 60;
  let secondsString = seconds |> string_of_int |> padNumber;
  minsString ++ ":" ++ secondsString;
};

[@react.component]
let make = () => {
  let (state, dispatch) =
    React.useReducer(
      (state, action) =>
        switch (action) {
        | Start => {...state, isTicking: true}
        | Stop => {...state, isTicking: false}
        | Reset =>
          updateTitle(formatTime(30));
          {...state, seconds: 30};
        | Tick =>
          state.isTicking && state.seconds > 0 ?
            {
              updateTitle(formatTime(state.seconds - 1));
              {...state, seconds: state.seconds - 1};
            } :
            {...state, isTicking: false}
        },
      {isTicking: false, seconds: 30},
    );

  React.useEffect0(() => {
    let timerId = Js.Global.setInterval(() => dispatch(Tick), 1000);
    Some(() => Js.Global.clearInterval(timerId));
  });

  <div className="container">
    <div className="timer">
      <p className="timer__text">
        {state.seconds |> formatTime |> ReasonReact.string}
      </p>
      {
        state.isTicking ?
          <div className="timer__buttons">
            <Button label="stop" onClick={_event => dispatch(Stop)} />
          </div> :
          <div className="timer__butons">
            {
              switch (state.seconds) {
              | 30 => {<Button label="start" onClick={_event => dispatch(Start)} />}
              | 0 => {<Button label="reset" onClick={_event => dispatch(Reset)} />}
              | _ => {
                  <>
                    <Button label="start" onClick={_event => dispatch(Start)} />
                    <Button label="reset" onClick={_event => dispatch(Reset)} />
                  </>
                }
              } 
            }
          </div>
      }
    </div>
  </div>;
};