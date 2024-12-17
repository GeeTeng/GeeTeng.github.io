function debounce(func, wait, options = {
    immediate: false,
    middle: true,
    thisArg: null,
  }) {
    let timer;
    let restDate = new Date();
    const immediate = options.immediate !== false;
    const middle = options.middle !== false;
    const thisArg = options.thisArg || null;
    return function (...args) {
      timer && clearTimeout(timer);
      let isFirst = !timer;
      timer = setTimeout(() => {
        func.apply(thisArg, args);
        restDate = new Date();
      }, wait);
      if ((new Date() - restDate > wait && middle) || (isFirst && immediate)) {
        clearTimeout(timer);
        func.apply(thisArg, args);
        restDate = new Date();
      }
    }
  }
  
  function setActive(anchors) {
    const ele = anchors.find((ele, index, arr) => {
      return ele.getBoundingClientRect().top >= 0 || index >= arr.length - 1;
    });
    if (ele) {
      const tableOfContents = document.querySelector('#table-of-contents');
      const toActive = tableOfContents.querySelector(`a[href="#${ele.id}"]`);
      if (!toActive) return;
      const activeA = tableOfContents.querySelector(`.active`);
      if (activeA) activeA.classList.remove('active');
      toActive.classList.add('active');
      window.history.pushState(null, null, `#${ele.id}`);
      tableOfContents.scrollTo({
        left: 0,
        top: toActive.offsetTop - tableOfContents.getBoundingClientRect().height / 2,
        behavior: 'smooth',
      });
    }
  }
  
  function initContents(icon = '<svg t="1690868184633" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="3381" width="32" height="32"><path d="M128 192l768 0 0 128-768 0 0-128Z" fill="#666666" p-id="3382"></path><path d="M128 448l768 0 0 128-768 0 0-128Z" fill="#666666" p-id="3383"></path><path d="M128 704l768 0 0 128-768 0 0-128Z" fill="#666666" p-id="3384"></path></svg>') {
    if(document.querySelector('#table-of-contents-wapper')) return;
    const contents = document.createElement('details');
    contents.id = 'table-of-contents-wapper';
    contents.innerHTML = `<summary>
      ${icon}
    </summary>`
    const styleElement = document.createElement('style')
    styleElement.innerHTML = `
    #table-of-contents-wapper {
      user-select: none;
      position: fixed;
      right: 1em;
      top: 4em;
      border-radius: 8px;
      z-index: 999
    }
  
    @media only screen and (min-width:768px) {
      #table-of-contents-wapper[open] summary {
        position: relative;
        left: 5.8em
      }
    }
  
    @media only screen and (max-width:768px) {
      #table-of-contents-wapper {
        top: auto;
        right: auto;
        bottom: 1.8rem;
        left: 1.8rem
      }
    }
  
    #table-of-contents-wapper summary {
      display: inline-block;
      font-size: 1.5em;
      border-radius: 4px;
      cursor: pointer;
      padding: .2em
    }
  
    #table-of-contents-wapper #table-of-contents {
      line-height: 1.3;
      width: 19rem;
      font-size: 15px;
      padding: .8em;
      border: 1px solid;
      border-radius: 6px;
      overflow-y: scroll;
      max-height: calc(100vh - 20rem);
      background-color: rgba(150, 150, 150, 0.8);
      color: #ffffff
    }
  
    #table-of-contents-wapper #table-of-contents a {
      width: 60%;
      display: inline-block;
      color: currentColor;
      line-height: 1.3;
      white-space: nowrap;
      overflow: hidden;
      text-overflow: ellipsis
    }
  
    #table-of-contents-wapper #table-of-contents .active {
      border-left: 2px solid #bedfff;
      color: #bedfff
    }
  
    .content h1,
    .content h2,
    .content h3 {
      padding-top: 2em !important;
      margin-top: -2em !important
    }`;
  
    const anchors = [...document.querySelector('.content').querySelectorAll('h1[id],h2[id],h3[id],h4[id]')];
    if (!anchors.length) return contents.remove();
    const tableOfContents = document.createElement('div');
    tableOfContents.id = 'table-of-contents';
    anchors.forEach(ele => {
      const a = document.createElement('a');
      if (!ele.innerText) return;
      a.innerText = ele.innerText;
      a.href = `#${ele.id}`;
      a.style.paddingLeft = `${ele.tagName.charAt(1)}em`;
      tableOfContents.appendChild(a);
    });
  
    contents.appendChild(tableOfContents);
    contents.open = window.innerWidth >= 768;
  
    document.head.appendChild(styleElement);
    document.body.append(contents);
  
    setActive(anchors);
    const debounceSetActive = debounce(setActive, 200)
    window.addEventListener('scroll', () => {
      debounceSetActive(anchors);
    });
  }
